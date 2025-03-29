#pragma GCC optimize ("-Os")
#pragma GCC push_options

// JSON optimization
#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_USE_LONG_LONG 0
#define ARDUINOJSON_ENABLE_STD_STRING 0
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1

/*
the file "ampx_modbus_functions.ino" should be in the same directory as this .ino file.
It is automatically included and merged with this file.
This just seperates all the modbus functions and make this file easier to read.

For The ESP32 Node32s board do the following:
Go to File -> Preferences.
in the "Additional Boards Manager URLs"
Add the following String:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Then in the boards manager, search for: "esp32 by Espressif Systems"
Then install the board manager.

Now select the port and then 
Select "node32s" under the boards.

*/

#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
//Search for Arduino Websockets, install the one by Markus Sattler
#include <WebSocketsServer.h>
//Search for ArduinoJson, install the one by Benoit Blanchon
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <Update.h>
//Install the one by Vlodomyr Shymanskyy, https://github.com/vshymanskyy/Preferences
#include <Preferences.h>
//Install the library by tzapu, https://github.com/tzapu/WiFiManager, https://www.youtube.com/watch?v=Errh7LEEug0
#include <WiFiManager.h>

//The HTML code is stored in a seperate file, this makes the code easier to read.
#include "webpage.h"
#include "web_settings.h"
//Define the meter registers and datatypes here
#include "meter_registers.h"

//Include AmpX custom written libraries for Modbus
//Saved in the D:\OneDrive\JF Data\UserData\Documents\Arduino\libraries folder
#include <ampx_modbus_tcpip.h>
#include <ampx_modbus_rs485.h>

//Unique Gateway ID for each gateway manufactured. To be used when adding it to a the portal under a specific user.
//Format: 25 02 0001 Year, Month, increment.
#define AMPX_GATEWAY_ID 202503040001

//Enable or disable debug output, set to 0 to disable debug output (saves memory)
#define DEBUG 0  
#if DEBUG == 1
  #define debug(...) Serial.print(__VA_ARGS__)
  #define debugln(...) Serial.println(__VA_ARGS__)
#else
  #define debug(...)
  #define debugln(...)
#endif

//There will be two variants of this gateway, one working with Modbus over RS485 and the other
//working with mobus over TCP/IP, this is setup here and used depeding on what is needed.
#define MODBUS_TYPE_RS485 1
#define MODBUS_TYPE_TCPIP 2
//Set the required variant here:
#define MODBUS_TYPE MODBUS_TYPE_TCPIP


//1.Define the RS485 control pins
#define MAX485_DE 4 //White
#define MAX485_RE_NEG 4
#define RX_PIN 16   //RO Orange
#define TX_PIN 17   //DI Yellow
//Modbus A, Positive, Green
//Modbus B, Negative, Bue


//2. Define Ethernet Pins, Mac and IPs
#define ETH_SPI_SCS   5   // CS (Chip Select), Green
// Network settings
byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB5};
IPAddress pc_ip(192, 168, 2, 120);   // PC IP
IPAddress ip(192, 168, 2, 121);     // Arduino IP
IPAddress gateway(192, 168, 2, 1);  // Network gateway
IPAddress subnet(255, 255, 255, 0); // Subnet mask
IPAddress meter_ip(192, 168, 2, 122); // Energy meter IP


//Define optimized JSON documents with minimum required size
StaticJsonDocument<512> JsonDoc;          //The JSonDocument is used to send data to the websocket.



//Define the Status indicating LEDs pins
#define LED_1_POWER 12
#define LED_2_METER 14
#define LED_3_WIFI 27
#define LED_4_INTERNET 26


//Constant data types, used in the processRegisters function.
//These are now defined in meter_registers.h
//const int dataTypeInt32 = 1;
//const int dataTypeInt64 = 2;
//const int dataTypeFloat = 3;


const char* firmwareURL = "https://ampx.co/downloads/ampx_open_energy_gateway.ino.bin";
bool readSerial = false;


//EMONCMS, Remote energy logging, https://JsonDocs.openenergymonitor.org/emoncms/index.html
const char* emoncms_server = "http://emoncms.org";
const char* api_key = "c0526f06893d1063800d3bb966927711"; //your_API_KEY

//AmpX Energy Portal, Remote energy logging
const char* ampxportal_server_local = "http://appampxco.local/";
const char* ampxportal_server_live = "https://app.ampx.co/";


String m1_serial_number = "";  // Meter one serial number
String m2_serial_number = "";
String m3_serial_number = "";
String m4_serial_number = "";  


int numberOfMeters = 0;  // Number of meters connected, this will automatically be updated based on the number of meters detected
int maxNumberOfMeters = 4;

#define HTTP 80
WebServer server(HTTP);
WebSocketsServer webSocket = WebSocketsServer(81);

Preferences preferences;

//Function prototypes, it needs to be here because it is used in the setup function.
//one needs to add a forward declaration for this function as well, as it is define in functions.ino
void handlePowerMeterRS485(int meterNumber);
void handlePowerMeterTCPIP(int meterNumber);
void postToAmpXPortal(int meterNumber);
void setupMeterRegisters();

void setup() {
  Serial.begin(9600); // Debug serial
    while (!Serial) {
    ; // Wait for serial port to connect
  }

  if (MODBUS_TYPE == MODBUS_TYPE_RS485) {
    // Initialize RS485 with Serial2
    rs485_init(&Serial2, RX_PIN, TX_PIN);
    debugln("RS485 Modbus initialized");
  } else {
    // MODBUS_TYPE = MODBUS_TYPE_TCPIP
    initEthernet();
  }

  // initialize LED status pins as outputs.
  pinMode(LED_1_POWER, OUTPUT);
  pinMode(LED_2_METER, OUTPUT);
  pinMode(LED_3_WIFI, OUTPUT);
  pinMode(LED_4_INTERNET, OUTPUT);

  //Indicate that the power is on with a LED
  digitalWrite(LED_1_POWER, HIGH);

  // Debugging information
  debug("AMPX_GATEWAY_ID: ");
  debugln(AMPX_GATEWAY_ID);
  debugln("Setup complete. Starting communication...");
  
  // Initialize meter register definitions
  setupMeterRegisters();

  // Initialize WiFi
  initWiFi(); //Program will not continue unless WiFi is connected..

  // Initialize NVS (Local Permanent Storage)
  initNvs();  

  //Initialise the local web server.
  initServer();

  //Detect number of meters and set global variable, numberOfMeters.
  detectNumberOfMeters();

  //Do the initial reading of the meters and update of the webpage, then repeat after 3 seconds in the loop.
  for (int i = 1; i <= numberOfMeters; i++) {
    if (MODBUS_TYPE == MODBUS_TYPE_RS485) {
      handlePowerMeterRS485(i); //Pass the meter number to the function. Updtate the values on the local web page.
    } else { // MODBUS_TYPE = MODBUS_TYPE_TCPIP
      handlePowerMeterTCPIP(i); //Pass the meter number to the function. Updtate the values on the local web page.
    }
    postToAmpXPortal(i); //Also post to the remote server as soon as the device starts up.
  }
  handleWebSocket();

}

void loop() {
  ArduinoOTA.handle();

  static unsigned long counter1 = 0;
  static unsigned long counter2 = 0;
  static unsigned long counter3 = 0;
  unsigned long now = millis();

  // Read the parameters every 3 seconds
  if (now - counter1 > 3000) {
    for (int i = 1; i <= numberOfMeters; i++) {
      if (MODBUS_TYPE == MODBUS_TYPE_RS485) {
        handlePowerMeterRS485(i); //Pass the meter number to the function. Updtate the values on the local web page.
      } else { // MODBUS_TYPE = MODBUS_TYPE_TCPIP
        handlePowerMeterTCPIP(i); //Pass the meter number to the function. Updtate the values on the local web page.
      }
    }
    handleWebSocket();
    counter1 = now;
  }

  // Post meter data to remote server every 5 minutes, 300000
  if (now - counter2 > 300000) {
    //Post meter data to remote server
    for (int i = 1; i <= numberOfMeters; i++) {
      //postToEmonCMS(i);
      postToAmpXPortal(i);
    }
    counter2 = now;
  }

  //Reboot every 24 hours to ensure it keeps working, 86400000
  if (now - counter3 > 86400000) {
    //Reboot the ESP32
    ESP.restart();
    //counter3 = now;
  }


  //These functions must run continuesly, so one can not include a delay in the main loop.
  server.handleClient();  //Handle webserver requests from client
  webSocket.loop();
}


