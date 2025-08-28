/*
The following .ino files should be in the same directory as this main .ino file (AmpX-Energy-Gateway.ino).
  ampx_functions.ino
  ampx_functions_modbus.ino
  ampx_functions_network.ino
  ampx_functions_web.ino

These files are automatically included and merged with this main .ino file.
This just seperates all the functions and make this file easier to read.


For The ESP32 Node32s board do the following:
Go to File -> Preferences.
in the "Additional Boards Manager URLs"
Add the following String:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Then in the boards manager, search for: "esp32 by Espressif Systems"
Then install the board manager.

Now select the port and then 
Select "node32s" under the boards.

//OTA Update has been disabled to reduce file size.
Go to Tools > Partition Scheme and select "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)" or "Huge APP (3MB No OTA/1MB SPIFFS)" if you don't need OTA.


*/
//Required to communicate with the RS485 Controller.
#include <HardwareSerial.h>

//Used to connect to the local WiFi network, Built-in library that comes with the ESP32 Arduino core
#include <WiFi.h>

//Install the library by tzapu, https://github.com/tzapu/WiFiManager, https://www.youtube.com/watch?v=Errh7LEEug0
#include <WiFiManager.h> //This is used to dymanically configure the wifi connection .

//Arduion Over the Air update functionality, commented out for now to try and save space
//#include <ArduinoOTA.h>
//#include <Update.h>

//Time library for NTP synchronization
#include <time.h>

//Install the one by Vlodomyr Shymanskyy, https://github.com/vshymanskyy/Preferences
//NVS  Non-Volatile Storage (Local Permanent Storage)
#include <Preferences.h>

//Creates a web server and serves a webpage with all the meter readings and settings.
#include <WebServer.h>

//Search for Arduino Websockets, install the one by Markus Sattler
#include <WebSocketsServer.h>

//Search for ArduinoJson, install the one by Benoit Blanchon
#include <ArduinoJson.h>

//Built-in ESP32 library that provides HTTP client functionality for making HTTP requests to web servers and APIs.
#include <HTTPClient.h>

//SPIFFS SPI Flash File System, this is used to store the web pages in the flash memory of the ESP32.
//This adds about 30% to the 1MB program file but moves and loads the html files from the SPIFFS memomory
#include "SPIFFS.h"

//The HTML code for the webpages are stored in a seperate file, this makes the code easier to read.
//#include "webpage.h"  //Moved to SPIFFS
//#include "web_settings.h" //Moved to SPIFFS
//#include "web_admin.h" //Moved to SPIFFS

//Define the meter registers and datatypes here in json format.
#include "meter_registers.h"

//Unique Gateway ID for each gateway manufactured. To be used when adding it to a the portal under a specific user.
//This is set when manufactured and will be unique for each gateway.
//Format: 100001 increment.
//TODO: JF 2025-05-05 This needs to be moved to the permanent settings and web page with admin settings created.
#define AMPX_GATEWAY_ID 100001



//This is an easy way to exclude all serial.print commands from production code to reduce the file size.
//Change this varialbe to enable or disable debugging
#define DEBUG 1

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
//Set the required Modbus type variant here to either RS485 or TCPIP
//In other words, change this depending on if the board is for RS485 or for TCPIP
#define MODBUS_TYPE MODBUS_TYPE_TCPIP


//Include AmpX custom written libraries for Modbus
//Saved in the D:\OneDrive\JF Data\UserData\Documents\Arduino\libraries folder
#if MODBUS_TYPE == MODBUS_TYPE_RS485
  
  //Custom written AmpX Modbus library for RS485
  //Saved in libary folder. On my pc: D:\OneDrive\JF Data\UserData\Documents\Arduino\libraries\ampx_modbus_rs485\src\ampx_modbus_rs485.cpp  
  #include <ampx_modbus_rs485.h>

  //1.Define the RS485 control pins
  #define MAX485_DE 4 //White
  #define MAX485_RE_NEG 4
  #define RX_PIN 16   //RO Orange
  #define TX_PIN 17   //DI Yellow
  //Modbus A, Positive, Green
  //Modbus B, Negative, Blue

#else

  //Custom written AmpX Modbus library for TCPIP
  //Saved in libary folder. On my pc: D:\OneDrive\JF Data\UserData\Documents\Arduino\libraries\ampx_modbus_tcpip\src\ampx_modbus_tcpip.cpp  
  #include <ampx_modbus_tcpip.h>

  //Define Ethernet Pins
  //SPI Interface Pin definitions for XIAO AND ESP32 WROOM 32U - Node32S, For W5500 and W5500-Lite
                                  //XIAO      WROOM
  #define ETH_SPI_SCS_PIN     5   //21,D6     5     // CS (Chip Select), nSS (On Datasheet) - Green wire - GPIO0 (any GPIO works for CS)
  //#define ETH_SPI_SCK_PIN   8   //8,D8      18    // SCK (SCLK), Clock - Yellow wire (hardware SPI)
  //#define ETH_SPI_MISO_PIN  9   //9,D9      19    // (MISO) - Orange wire (hardware SPI)
  //#define ETH_SPI_MOSI_PIN  10  //10,D10    23    // (MOSI) - Blue wire  (hardware SPI)
  //Red     VCC   3V
  //Black   GND   On row two for ground

  //Define Ethernet Settings, Mac and IPs
  byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x94, 0xB5};
  IPAddress pc_ip(192, 168, 2, 32);   // PC IP, Assign a static IP to your PC and change it here to be the same.
  IPAddress ip(192, 168, 2, 121);     // Arduino IP
  IPAddress gateway(192, 168, 2, 1);  // Network gateway
  IPAddress subnet(255, 255, 255, 0); // Subnet mask
  IPAddress meter_ip(192, 168, 2, 122); // Energy meter IP

#endif


//Define the status indicating LEDs pins
//                        //C3  //WROOM
#define LED_1_POWER     12 //2   //12 //Indicates Power is on 
#define LED_2_METER     14 //3   //14 //Indicates Meter is connected via Modbus
#define LED_3_WIFI      27 //4   //27 //Indicates WiFi is connected
#define LED_4_INTERNET  26 //5   //26 //Indicates Internet is connected, this is the green LED
#define LED_5_SERVER    25 //6   //25 //Indicates succesfull communication with the Server


//Constant data types, used in the processRegisters function.
//These are now defined in meter_registers.h
//const int dataTypeInt32 = 1;
//const int dataTypeInt64 = 2;
//const int dataTypeFloat = 3;

//NVS  Non-Volatile Storage (Local Permanent Storage)
Preferences preferences;

//Web server and websocket
#define HTTP 80
WebServer server(HTTP);
WebSocketsServer webSocket = WebSocketsServer(81);


//TODO: Improve this code use an array and do not use strings.
String m1_serial_number = "";  // Meter one serial number
String m2_serial_number = "";
String m3_serial_number = "";
String m4_serial_number = "";  


int numberOfMeters = 0;  // Number of meters connected, this will automatically be updated based on the number of meters detected
int maxNumberOfMeters = 4;


//The JsonDocument is used to send data to the websocket....
//StaticJsonDocument<512> JsonDoc;          //Define optimized JSON documents with minimum required size. StaticJsonDocument allocates memory on the stack (fixed at compile time)
DynamicJsonDocument JsonDoc(2048);        //DynamicJsonDocument allocates memory on the heap (dynamic at runtime)

//JSON document for meter register definitions
JsonDocument MeterRegisterDefs;


//Firmware URL to use to update the firmware via OTA
const char* firmwareURL = "https://ampx.co/downloads/ampx_open_energy_gateway.ino.bin";
bool readSerial = false;


//EMONCMS, Remote energy logging, https://JsonDocs.openenergymonitor.org/emoncms/index.html
//TODO This needs to be removed, can forward to other portal from ampx portal
/*Commented out to save space
const char* emoncms_server = "http://emoncms.org";
const char* api_key = "c0526f06893d1063800d3bb966927711"; //your_API_KEY
*/

//AmpX Energy Portal, Remote energy logging
//TODO This needs to be saved in persistent memory and moved to the web admin settings page.
//const char* ampxportal_server_local = "http://192.168.2.32:8080/api/";
const char* ampxportal_server_local = "http://192.168.2.32:8080/api/v2/";
const char* ampxportal_server_live = "https://ampx.app/api/v2/"; //"https://portal.ampx.app/api/v2/"; //old "https://app.ampx.co/api/v2/";


// Which API to use local or live? - set to true for local development, false for live
//TODO: 2025-08-26 This needs to be moved to the preferences and web admin settings page.
#define USE_LOCAL_SERVER true


//Function prototypes, it needs to be here because it is used in the setup function.
//one needs to add a forward declaration for this function as well, as it is defined in a seperate .ino file:functions.ino
void handlePowerMeter(int meterNumber);
void postToAmpXPortal2(int meterNumber);
void setupMeterRegisters();
void detectNumberOfMeters();
void handleWebSocket();
void initNTP();
String getCurrentTimestamp();

void setup() {
  // initialize LED status pins as outputs.
  pinMode(LED_1_POWER, OUTPUT);
  pinMode(LED_2_METER, OUTPUT);
  pinMode(LED_3_WIFI, OUTPUT);
  pinMode(LED_4_INTERNET, OUTPUT);

  //Indicate that the power is on with a LED
  digitalWrite(LED_1_POWER, HIGH);

  Serial.begin(9600); // Debug serial
  while (!Serial) {
    delay(10); // Wait for serial port to become ready.
  }
  delay(1200); //Wait some more for the serial port to become ready...
  debugln("Serial port ready. Begin setup...");
  
  // Debuging information
  debug("AMPX_GATEWAY_ID: ");
  debugln(AMPX_GATEWAY_ID);

  //Depending on the Modbus type set, initialise either RS485 or TCPIP
  #if MODBUS_TYPE == MODBUS_TYPE_RS485
    initRS485(&Serial1, RX_PIN, TX_PIN);
    debugln("RS485 Modbus initialized");
  #else
    initEthernet();
    debugln("TCPIP Modbus initialized");
  #endif
  
  Serial.println("Setup complete. Starting communication...");
  
  // Initialize WiFi
  initWiFi(); //Program will not continue unless WiFi is connected..

  //Do Over the air update for firmware updates
  initOTA();

  // Initialize NTP time synchronization (must be after WiFi)
  initNTP();

  // Initialize NVS  Non-Volatile Storage (Local Permanent Storage)
  initNvs();  

  //Initialise the local web server.
  initServer();

  // Initialize meter register definitions
  setupMeterRegisters();

  //Detect number of meters and set global variable, numberOfMeters.
  detectNumberOfMeters();

  //Do the initial reading of the meters and update of the webpage, then repeat after 3 seconds in the loop.
  for (int i = 1; i <= numberOfMeters; i++) {

    //JF: New fuction to handle both RS485 and TCPIP connections to meters.
    handlePowerMeter(i);
    
    postToAmpXPortal2(i); //Also post to the remote server as soon as the device starts up.
  }
  //Update the webpage through the websocket with the meter data.
  handleWebSocket();

}

void loop() {
  //JF commented out to reduce size
  //ArduinoOTA.handle();

  static unsigned long counter1 = 0;
  static unsigned long counter2 = 0;
  static unsigned long counter3 = 0;
  
  //Interval to test the meter connection and read the parameters, and update the local web page.
  const unsigned long METER_CONNECTION_INTERVAL = 3000;    // 3 seconds
  //Interval to post the meter data to the remote server
  const unsigned long REMOTE_SERVER_INTERVAL = 30000;    // 30 seconds
  //Interval to reboot the ESP32
  const unsigned long REBOOT_INTERVAL = 86400000;    // 24 hours

  //Built-in Arduino function that returns the number of milliseconds since the Arduino board began running the current program.
  unsigned long now = millis();

  //Test meter connection and read the parameters every 3 seconds
  if (now - counter1 > METER_CONNECTION_INTERVAL) {
    //Test if the meter is still connected.
    if (modbus_test_connection()) {
      debugln("Connection test successful! We are able to communicate with the meter with modbus over TCPIP!");
      //Turn on LED 2 to indicate successful connection to energy meter.
      digitalWrite(LED_2_METER, HIGH);

      //Read the parameters from the meter and update the local web page.
      for (int i = 1; i <= numberOfMeters; i++) {
        //JF: New fuction to handle both RS485 and TCPIP
        handlePowerMeter(i);
      }
      handleWebSocket();
  
    } else {
      debugln("Connection test failed!");
      //Turn off LED 2 to indicate unsuccessful connection to energy meter.
      digitalWrite(LED_2_METER, LOW);  
      //Try and reconnect...
      //Depending on the Modbus type set, initialise either RS485 or TCPIP
      #if MODBUS_TYPE == MODBUS_TYPE_RS485
        initRS485(&Serial1, RX_PIN, TX_PIN);
        debugln("RS485 Modbus initialized");
      #else
        initEthernet();
        debugln("TCPIP Modbus initialized");
      #endif
    }
    //Reset the counter
    counter1 = now;
  }

  // Post meter data to remote server every 5 minutes, 300000
  //TODO: JF 2025-06-08 for testing, i decreased it to 30 seconds (30000).
  if (now - counter2 > REMOTE_SERVER_INTERVAL) {
    //Post meter data to remote server
    for (int i = 1; i <= numberOfMeters; i++) {
      //postToEmonCMS(i);
      postToAmpXPortal2(i);
    }
    counter2 = now;
  }

  //Reboot every 24 hours to ensure it keeps working, 86400000
  if (now - counter3 > REBOOT_INTERVAL) {
    //Reboot the ESP32
    //ESP.restart();
    counter3 = now;
  }


  //These functions must run continuesly, so one can not include a delay in the main loop.
  server.handleClient();  //Handle webserver requests from client
  webSocket.loop();       //Handle websocket requests from client
}


