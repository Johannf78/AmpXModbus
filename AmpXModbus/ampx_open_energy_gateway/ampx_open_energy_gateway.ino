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
//Required to communicate with the RS485 Controller.
#include <HardwareSerial.h>
//Connect to the local WiFi network
#include <WiFi.h>
//Creates a web server and serves a webpage with all the meter readings.
//Install the library by tzapu, https://github.com/tzapu/WiFiManager, https://www.youtube.com/watch?v=Errh7LEEug0
#include <WiFiManager.h> //This is used to dymanically configure the wifi connection .
#include <WebServer.h>
#include <HTTPClient.h>
//Search for Arduino Websockets, install the one by Markus Sattler
#include <WebSocketsServer.h>
//Search for ArduinoJson, install the one by Benoit Blanchon
#include <ArduinoJson.h>
//Time library for NTP synchronization
#include <time.h>
//Arduion Over the Air update functionality
//#include <ArduinoOTA.h>
//#include <Update.h>
//Install the one by Vlodomyr Shymanskyy, https://github.com/vshymanskyy/Preferences
#include <Preferences.h>


//The HTML code is stored in a seperate file, this makes the code easier to read.
#include "webpage.h"
#include "web_settings.h"
#include "web_admin.h"
//Define the meter registers and datatypes here
#include "meter_registers.h"

//Unique Gateway ID for each gateway manufactured. To be used when adding it to a the portal under a specific user.
//Format: 25 02 0001 Year, Month, increment.
//TODO: JF 2025-05-05 This needs to be moved to the permanent settings and web page with admin settings created.
#define AMPX_GATEWAY_ID 100001

#define DEBUG 0
#if DEBUG == 1
  #define debug(x) Serial.print(x)
  #define debugln(x) Serial.println(x)
#else
  #define debug(x)
  #define debugln(x)
#endif

//There will be two variants of this gateway, one working with Modbus over RS485 and the other
//working with mobus over TCP/IP, this is setup here and used depeding on what is needed.
#define MODBUS_TYPE_RS485 1
#define MODBUS_TYPE_TCPIP 2
//Set the required variant here:________________________________________________________
#define MODBUS_TYPE MODBUS_TYPE_TCPIP

//Include AmpX custom written libraries for Modbus
//Saved in the D:\OneDrive\JF Data\UserData\Documents\Arduino\libraries folder
#if MODBUS_TYPE == MODBUS_TYPE_RS485
  #include <ampx_modbus_rs485.h>
#else
  #include <ampx_modbus_tcpip.h>
#endif

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
IPAddress pc_ip(192, 168, 2, 32);   // PC IP, Assign a static IP to your PC and change it here to be the same.
IPAddress ip(192, 168, 2, 121);     // Arduino IP
IPAddress gateway(192, 168, 2, 1);  // Network gateway
IPAddress subnet(255, 255, 255, 0); // Subnet mask
IPAddress meter_ip(192, 168, 2, 122); // Energy meter IP


//The JsonDocument is used to send data to the websocket....
//StaticJsonDocument<512> JsonDoc;          //Define optimized JSON documents with minimum required size. StaticJsonDocument allocates memory on the stack (fixed at compile time)
DynamicJsonDocument JsonDoc(2048);        //DynamicJsonDocument allocates memory on the heap (dynamic at runtime)

//JSON document for meter register definitions
JsonDocument MeterRegisterDefs;

//Define the Status indicating LEDs pins, Commented for 
#define LED_1_POWER 12  //Indicates Power is on
#define LED_2_METER 14  //Indicates Meter is connected via Modbus
#define LED_3_WIFI 27   //Indicates WiFi is connected
#define LED_4_INTERNET 26 //Indicates Internet is connected
#define LED_5_SERVER 28 //Indicates succesfull communication with the Server

//FritzFamily WiFi 03368098169909319946

//Constant data types, used in the processRegisters function.
//These are now defined in meter_registers.h
//const int dataTypeInt32 = 1;
//const int dataTypeInt64 = 2;
//const int dataTypeFloat = 3;


const char* firmwareURL = "https://ampx.co/downloads/ampx_open_energy_gateway.ino.bin";
bool readSerial = false;


//EMONCMS, Remote energy logging, https://JsonDocs.openenergymonitor.org/emoncms/index.html
//TODO This needs to be removed, can forward to other portal from ampx portal
const char* emoncms_server = "http://emoncms.org";
const char* api_key = "c0526f06893d1063800d3bb966927711"; //your_API_KEY

//AmpX Energy Portal, Remote energy logging
//TODO This needs to be saved in persistent memory and moved to the web admin settings page.
//const char* ampxportal_server_local = "http://192.168.2.32:8080/api/";
const char* ampxportal_server_local = "http://192.168.2.32:8080/api/v2/";
const char* ampxportal_server_live = "https://portal.ampx.app/api/v2/"; //old "https://app.ampx.co/api/v2/";


// Development flag - set to true for local development, false for production
#define USE_LOCAL_SERVER true


//TODO: Improve this code use an array and do not use strings.
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
//one needs to add a forward declaration for this function as well, as it is defined in a seperate .ino file:functions.ino
//void handlePowerMeterRS485(int meterNumber);
//void handlePowerMeterTCPIP(int meterNumber);
void handlePowerMeter(int meterNumber);
void postToAmpXPortal2(int meterNumber);
void setupMeterRegisters();
void detectNumberOfMeters();
void handleWebSocket();
void initNTP();
String getCurrentTimestamp();

void setup() {
  Serial.begin(9600); // Debug serial
    while (!Serial) {
    ; // Wait for serial port to connect
  }

  // initialize LED status pins as outputs.
  pinMode(LED_1_POWER, OUTPUT);
  pinMode(LED_2_METER, OUTPUT);
  pinMode(LED_3_WIFI, OUTPUT);
  pinMode(LED_4_INTERNET, OUTPUT);

  //Indicate that the power is on with a LED
  digitalWrite(LED_1_POWER, HIGH);

  // Debugging information
  Serial.print("AMPX_GATEWAY_ID: ");
  Serial.println(AMPX_GATEWAY_ID);

  /*
  if (MODBUS_TYPE == MODBUS_TYPE_RS485) {
    // Initialize RS485 with Serial2
    //Serial 2 for ESP32Wroom, Serial 1 for XIAO
    rs485_init(&Serial1, RX_PIN, TX_PIN);
    Serial.println("RS485 Modbus initialized");
  } else {
    // MODBUS_TYPE = MODBUS_TYPE_TCPIP
    //JF: Commented to reduce file size, uncomment when using tcpip
    initEthernet();
  }
  */

  #if MODBUS_TYPE == MODBUS_TYPE_RS485
    rs485_init(&Serial1, RX_PIN, TX_PIN);
    Serial.println("RS485 Modbus initialized");
  #else
    initEthernet();
  #endif
  
  Serial.println("Setup complete. Starting communication...");
  
  // Initialize meter register definitions
  setupMeterRegisters();

  // Initialize WiFi
  initWiFi(); //Program will not continue unless WiFi is connected..

  // Initialize NTP time synchronization (must be after WiFi)
  initNTP();

  // Initialize NVS (Local Permanent Storage)
  initNvs();  

  //Initialise the local web server.
  initServer();

  //Detect number of meters and set global variable, numberOfMeters.
  detectNumberOfMeters();

  //Do the initial reading of the meters and update of the webpage, then repeat after 3 seconds in the loop.
  for (int i = 1; i <= numberOfMeters; i++) {
    /*
    if (MODBUS_TYPE == MODBUS_TYPE_RS485) {
      handlePowerMeterRS485(i); //Pass the meter number to the function. Updtate the values on the local web page.
    } else { // MODBUS_TYPE = MODBUS_TYPE_TCPIP
      handlePowerMeterTCPIP(i); //Pass the meter number to the function. Updtate the values on the local web page.
    }
    */
    //JF: New fuction to handle both RS485 and TCPIP
    handlePowerMeter(i);
    


    postToAmpXPortal2(i); //Also post to the remote server as soon as the device starts up.
  }
  handleWebSocket();

}

void loop() {
  //JF commented out to reduce size
  //ArduinoOTA.handle();

  static unsigned long counter1 = 0;
  static unsigned long counter2 = 0;
  static unsigned long counter3 = 0;
  unsigned long now = millis();

  // Read the parameters every 3 seconds
  if (now - counter1 > 3000) {
    for (int i = 1; i <= numberOfMeters; i++) {
      /*
      if (MODBUS_TYPE == MODBUS_TYPE_RS485) {
        handlePowerMeterRS485(i); //Pass the meter number to the function. Updtate the values on the local web page.
      } else { // MODBUS_TYPE = MODBUS_TYPE_TCPIP
        handlePowerMeterTCPIP(i); //Pass the meter number to the function. Updtate the values on the local web page.
      }*/
       //JF: New fuction to handle both RS485 and TCPIP
      handlePowerMeter(i);
    }
    handleWebSocket();
    counter1 = now;
  }

  // Post meter data to remote server every 5 minutes, 300000
  //TODO: JF 2025-06-08 for testing, i decreased it to 30 seconds (30000).
  if (now - counter2 > 30000) {
    //Post meter data to remote server
    for (int i = 1; i <= numberOfMeters; i++) {
      //postToEmonCMS(i);
      postToAmpXPortal2(i);
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


