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

//The JSonDocument is used to send data to the websocket.
JsonDocument JsonDoc;

// Define the RS485 control pins
#define MAX485_DE 4
#define MAX485_RE_NEG 4
#define RX_PIN 16
#define TX_PIN 17
#define HTTP 80

//Constant data types
const int dataTypeInt32 = 1;
const int dataTypeInt64 = 2;
const int dataTypeFloat = 3;

const char* ssid = "FRITZ!Family";
const char* password = "03368098169909319946";

//const char* ssid = "RUT901";
//const char* password = "d9U8DyWb";

//const char* ssid = "Telkom";
//const char* password = "0827270909";

//const char* ssid = "Poly";
//const char* password = "polypassword";

const char* firmwareURL = "https://ampx.co/downloads/ampx_open_energy_gateway.ino.bin";
bool readSerial = false;


//EMONCMS, Remote energy logging, https://JsonDocs.openenergymonitor.org/emoncms/index.html
const char* emoncms_server = "http://emoncms.org";
const char* api_key = "c0526f06893d1063800d3bb966927711"; //your_API_KEY

String m1_serial_number = "";  // Meter one serial number
String m2_serial_number = "";
String m3_serial_number = "";
String m4_serial_number = "";  


int numberOfMeters = 1;  // Number of meters connected, this needs to be adjusted if more meters are connected.
int maxNumberOfMeters = 4;

WebServer server(HTTP);
WebSocketsServer webSocket = WebSocketsServer(81);

Preferences preferences;
/*
enum floatInt32orInt64
{
  isfloat,isInt32,isInt64
}
*/

void initNvs() {
  if (preferences.begin("app", false)) {
    Serial.println("NVS initialized successfully!");
  } else {
    Serial.println("Failed to initialize NVS.");
  }
}

void initWiFi() {
  
  WiFi.setHostname("AmpX-Energy-Gateway");

  // Connect to the Wi-Fi network
  /*WiFi.begin(ssid, password);

  Serial.print("Now connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");//Print more dots while connecting 
  }*/

  WiFiManager wifiManager;

  // If you've previously connected to your WiFi with this ESP32,
  // WiFi manager will more than likely not do anything.
  // Uncomment this if you want to force it to delete your old WiFi details.
  //wifiManager.resetSettings();

  //Tries to connect to last known WiFi details
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AmpX-Energy-Gateway-AP", "")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  Serial.println("");
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal strength: ");
  Serial.println(WiFi.RSSI());
  Serial.print("WiFi.getHostname: ");
  Serial.println(WiFi.getHostname());

  // OTA setup
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
    
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress * 100) / total);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();
  Serial.println("Ready for OTA updates.");
}

void initServer() {
  server.on("/", handleRoot);
  server.on("/update", HTTP_POST, handleUpdate);
  server.on("/settings", handleSettings);
  server.on("/update_meters_name", HTTP_POST, handleChangeMetersName);
  server.begin();
  //Initialise the websockets on port 81
  webSocket.begin();
}

void handleChangeMetersName() {
  String m1_name = server.arg("m1_name");
  preferences.putString("m1_name", m1_name);
  Serial.println("m1_name: " + m1_name);

  String m2_name = server.arg("m2_name");
  preferences.putString("m2_name", m2_name);
  Serial.println("m2_name: " + m2_name);

  String m3_name = server.arg("m3_name");
  preferences.putString("m3_name", m3_name);
  Serial.println("m3_name: " + m3_name);

  String m4_name = server.arg("m4_name");
  preferences.putString("m4_name", m4_name);
  Serial.println("m4_name: " + m4_name);


  server.send(200, "text/html", "Updated successfully. <br><a href='/settings'>settings</a>");
}

void handleSettings()
{
  String page = webpage_settings;
  //Use javascript to hide settings for meters not present.
  page.replace("numberOfMetersValue", String(numberOfMeters));

  String m1_name = preferences.getString("m1_name");
  Serial.println("m1_name_value: " + m1_name);
  page.replace("m1_name_value", m1_name);

  String m2_name = preferences.getString("m2_name");
  Serial.println("m2_name_value: " + m2_name);
  page.replace("m2_name_value", m2_name);

    //Replace the string m1_serial_number with the actual serial number, done here as it does not update regularly like values.
  page.replace("m1_serial_number", m1_serial_number);
  page.replace("m2_serial_number", m2_serial_number);
  page.replace("m3_serial_number", m3_serial_number);

  server.send(200, "text/html", page);
}

void handleRoot() {
  //String html = "<h1>AmpX Open Energy Gateway</h1>";
  //String vol = "<h1>Voltage on L1: " + String(voltage_on_L1, 2) + "(V)</h1>";
  //html += vol;
  //the String webpage has been defined in the included file webpage.h

  //Replace the string m1_serial_number with the actual serial number, done here as it does not update regularly like values.
  webpage.replace("m1_serial_number", m1_serial_number);
  webpage.replace("m2_serial_number", m2_serial_number);
  webpage.replace("m3_serial_number", m3_serial_number);
  webpage.replace("numberOfMetersValue", String(numberOfMeters));

  /*
  for (int i = 1; i <= maxNumberOfMeters; i++)
  {
    String meterPrefix = "m" + String(i) + "_";
    //Copy the saved meter name from the preferences to the json doc, this is so that the name can be displayed on the page, even if the name has been changed recently.
    JsonDoc[meterPrefix + "name"] = preferences.getString(meterPrefix + "name");
  }
  */
  JsonDoc["m1_name"] = preferences.getString("m1_name");
  JsonDoc["m2_name"] = preferences.getString("m2_name");
  JsonDoc["m3_name"] = preferences.getString("m3_name");
  JsonDoc["m4_name"] = preferences.getString("m4_name");

  server.send(200, "text/html", webpage);
}

void handleUpdate() {
  doOTAUpdate();
}

void processRegisters(uint16_t* results, uint16_t numRegisters, int registerDataType,
                      const String& friendlyLabel, const String& docLabel) {

  Serial.println("");
  Serial.println(friendlyLabel);  
  for (uint16_t i = 0; i < numRegisters; i++) {
    Serial.print("Register " + String(i) + ": ");
    Serial.println(results[i]);
  }
  
  float floatValue = 0;
  uint32_t intValue = 0;
  String stringValue = "";

  if (registerDataType == dataTypeInt32){
    Serial.println("dataTypeInt32");
    intValue = combineRegistersToInt32(results[0], results[1]);
    stringValue = String(intValue); //convert integer to sting
  }
  if (registerDataType == dataTypeInt64){
    Serial.println("dataTypeInt64");
    
  }
  if (registerDataType == dataTypeFloat){
    Serial.println("dataTypeFloat");
    floatValue = combineRegistersToFloat(results[0], results[1]);
    stringValue = String(floatValue,2); //convert float to string with two decimal places
  }
/*
  //uint32_t combinedValue = combineAndSwap(results[0], results[1]);

  //float value = convertToFloat(combinedValue);

  //int value = combineRegistersToInt32(results[0], results[1]);

  //String value = "Test";

  //Update the json document with the value
  //JsonDoc[docLabel] = value;

  */
  Serial.print(friendlyLabel);
  Serial.print(": ");
  Serial.println(stringValue);
  
}

//TODO: Delete this function
void processRegistersInt64(uint16_t* responseBuffer, uint16_t numRegisters,
                           const String& friendlyLabel, const String& docLabel) {
  // Print registers in hex
  /*
    Serial.print("Registers (hex): ");
    for(int i = 0; i < numRegisters; i++) {
        Serial.print("0x");
        Serial.print(responseBuffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    */

  uint64_t value = combineAndSwap64(responseBuffer[0], responseBuffer[1],
                                    responseBuffer[2], responseBuffer[3]);

  /*
    Serial.print("Energy: ");
    Serial.println(value);
    Serial.print("Energy (hex): 0x");
    Serial.println((unsigned long)energy, HEX);
    */

  //Update the json document with the value
  JsonDoc[docLabel] = String((float)value / 1000, 2);

  //Serial.print(friendlyLabel);
  //Serial.print(": ");
  //Serial.println(value);
}



void handlePowerMeter(int meterNumber = 1) {
  //These variable are populated from the data read on Modbus, they are reused for different parameters, voltage, current, power, etc.
  // uint16_t results[32];
  uint16_t responseBuffer[4];
  //float value;

  String meterPrefix = "m" + String(meterNumber) + "_";

   // Read Serial number, registers 70 and 71.
  if (readHoldingRegisters(meterNumber, 70, 2, responseBuffer)) { // meterNumber is the Modbus slave ID
    
    Serial.println("");
    Serial.println("Serial of meterNumber: " + String(meterNumber));
    Serial.print("Register 70: ");
    Serial.println(responseBuffer[0]);
    Serial.print("Register 71: ");
    Serial.println(responseBuffer[1]);
    uint32_t combinedValue = combineRegistersToInt32(responseBuffer[0], responseBuffer[1]);
    Serial.print("Serial number: ");
    Serial.println(combinedValue);

    processRegisters(responseBuffer, 2, dataTypeInt32, "Serial number", meterPrefix + "serial");
    
      
    if (meterNumber == 1){
      m1_serial_number = combinedValue;
    }
    if (meterNumber == 2){
      m2_serial_number = combinedValue;
    }
    if (meterNumber == 3){
      m3_serial_number = combinedValue;
    }
    if (meterNumber == 4){
      m4_serial_number = combinedValue;
    }
    
    } else {
    Serial.println("Error reading serial number");
  }
 

  // Read voltage on L1
  if (readHoldingRegisters(meterNumber, 1010, 2, responseBuffer)) { // meterNumber is the Modbus slave ID
    processRegisters(responseBuffer, 2, dataTypeFloat, "Voltage L1", meterPrefix + "voltage_L1");
    } else {
    Serial.println("Error reading voltage registers");
  }
  // Read voltage on L2
  if (readHoldingRegisters(meterNumber, 1012, 2, responseBuffer)) {  // meterNumber is the Modbus slave ID
    processRegisters(responseBuffer, 2, dataTypeFloat, "Voltage L2", meterPrefix + "voltage_L2");
  } else {
    Serial.println("Error reading voltage registers");
  }
  // Read voltage on L3
  if (readHoldingRegisters(meterNumber, 1014, 2, responseBuffer)) { 
    processRegisters(responseBuffer, 2, dataTypeFloat, "Voltage L3", meterPrefix + "voltage_L3");
  } else {
    Serial.println("Error reading voltage registers");
  }


  // Read current on L1
  if (readHoldingRegisters(meterNumber, 1000, 2, responseBuffer)) { 
    processRegisters(responseBuffer, 2, dataTypeFloat, "Current L1", meterPrefix + "current_L1");
  } else {
    Serial.println("Error reading current registers");
  }
  // Read current on L2
  if (readHoldingRegisters(meterNumber, 1002, 2, responseBuffer)) { 
    processRegisters(responseBuffer, 2, dataTypeFloat, "Current L2", meterPrefix + "current_L2");
  } else {
    Serial.println("Error reading current registers");
  }
  // Read current on L3
  if (readHoldingRegisters(meterNumber, 1004, 2, responseBuffer)) {
    processRegisters(responseBuffer, 2, dataTypeFloat, "Current L3", meterPrefix + "current_L3");
  } else {
    Serial.println("Error reading current registers");
  }
  // Read current average
  if (readHoldingRegisters(meterNumber, 1006, 2, responseBuffer)) {
    processRegisters(responseBuffer, 2, dataTypeFloat, "Current Avg", meterPrefix + "current_avg");
  } else {
    Serial.println("Error reading current registers");
  }


  // Read Active Power on L1
  if (readHoldingRegisters(meterNumber, 1028, 2, responseBuffer)) { 
    processRegisters(responseBuffer, 2, dataTypeFloat, "Active Power L1", meterPrefix + "active_power_L1");
  } else {
    Serial.println("Error reading Active Power registers");
  }
  // Read Active Power on L2
  if (readHoldingRegisters(meterNumber, 1030, 2, responseBuffer)) { 
    processRegisters(responseBuffer, 2, dataTypeFloat, "Active Power L2", meterPrefix + "active_power_L2");
  } else {
    Serial.println("Error reading Active Power registers");
  }
  // Read Power on L3
  if (readHoldingRegisters(meterNumber, 1032, 2, responseBuffer)) { 
    processRegisters(responseBuffer, 2, dataTypeFloat, "Active Power L3", meterPrefix + "active_power_L3");
  } else {
    Serial.println("Error reading Active Power registers");
  }
  // Read Total Power
  if (readHoldingRegisters(meterNumber, 1034, 2, responseBuffer)) { 
    processRegisters(responseBuffer, 2, dataTypeFloat, "Active Power Total", meterPrefix + "active_power_tot");
  } else {
    Serial.println("Error reading Active Power registers");
  }
/*
  //Read Active Energy Imported L1
  if (readHoldingRegisters64(meterNumber, 2500, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, dataTypeInt64, "Energy Imported L1", meterPrefix + "active_energy_imported_L1");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  //Read Active Energy Imported L2
  if (readHoldingRegisters64(meterNumber, 2504, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, dataTypeInt64, "Energy Imported L2", meterPrefix + "active_energy_imported_L2");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  //Read Active Energy Imported L3
  if (readHoldingRegisters64(meterNumber, 2508, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, dataTypeInt64, "Energy Imported L3", meterPrefix + "active_energy_imported_L3");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  //Read Active Energy Imported Total
  if (readHoldingRegisters64(meterNumber, 2512, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, dataTypeInt64, "Energy Imported Total", meterPrefix + "active_energy_imported_tot");
     processRegistersInt64(responseBuffer, 4, dataTypeInt64, "Energy Imported Total", meterPrefix + "active_energy_imported_tot_summary");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  */
}

void handleWebSocket() {
  String JsonString;

  serializeJson(JsonDoc, JsonString);
  //Send the JSON document to the websocket.
  webSocket.broadcastTXT(JsonString);

  //TODO Serial.println("Sent JSON to websocket");
  //TODO Serial.println(JsonString);
}

void postToRemoteServer(int meterNumber = 1) { 
  
  String meterPrefix = "m" + String(meterNumber) + "_";

/* for testing 
  int test_power1 = random(1000);
  int test_power2 = random(1000);
  int test_power3 = random(1000);
  int test_powert = random(1000);

  String valuesString ="";
  valuesString = "power1:" + String(test_power1);
  valuesString += ",power2:" + String(test_power2);
  valuesString += ",power3:" + String(test_power3);
  valuesString += ",powert:" + String(test_powert);
//  Serial.println("valuesString:  " + valuesString);
*/

  String valuesString2 ="";
  valuesString2 = "power1:" + String(JsonDoc[meterPrefix + "active_power_L1"]);
  valuesString2 += ",power2:" + String(JsonDoc[meterPrefix + "active_power_L2"]);
  valuesString2 += ",power3:" + String(JsonDoc[meterPrefix + "active_power_L3"]);
  valuesString2 += ",powert:" + String(JsonDoc[meterPrefix + "active_power_tot"]);
 // Serial.println("valuesString2:  " + valuesString2);

  //Add energy imported values
  valuesString2 += ",energy1:" + String(JsonDoc[meterPrefix + "active_energy_imported_L1"]);
  valuesString2 += ",energy2:" + String(JsonDoc[meterPrefix + "active_energy_imported_L2"]);
  valuesString2 += ",energy3:" + String(JsonDoc[meterPrefix + "active_energy_imported_L3"]);
  valuesString2 += ",energyt:" + String(JsonDoc[meterPrefix + "active_energy_imported_tot"]);
  Serial.println("valuesString2:  " + valuesString2);


  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "";
    url = String(emoncms_server);
    url += "/input/post?node=" + String(meterNumber) + "&json={";
    //Test with fixed values
    //url += "power1:100,power2:200,power3:300";
    //Test with random values
    //url += valuesString;
    //Test with actual values
    url += valuesString2;
    url += "}&apikey=" + String(api_key);
    //Serial.println(url);

    

    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      //Serial.println(httpResponseCode);
      Serial.print("Https client Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
      //Responce code -1 means no internet access, data ok?
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void detectNumberOfMeters(){
  uint16_t responseBuffer[4];
  //Find number of meters, 4 max number for now.
  for (int i = 1; i <= maxNumberOfMeters; i++) {
    // Read Serial number registers 70 and 71
    if (readHoldingRegisters(i, 70, 2, responseBuffer)) {  // i is the Modbus slave ID
      uint32_t combinedValue = combineRegistersToInt32(responseBuffer[0], responseBuffer[1]);
      //Serial.print("Serial Number: ");
      //Serial.println(combinedValue);
      //Update the number of meters if able to read its serial number
      numberOfMeters = i;
      Serial.println("Number of meters detected: " + String(i));      
    } else {
     Serial.println("Error reading meter: " + String(i));
    }
  } 
}

void doOTAUpdate() {
  Serial.println("Starting OTA update...");

  HTTPClient http;
  http.begin(firmwareURL);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    int length = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    if (!Update.begin(length)) {
      Serial.println("Not enough space for OTA update");
      return;
    }

    // Start updating
    size_t written = Update.writeStream(*stream);
    if (written == length) {
      Serial.println("Firmware written successfully!");
      //TODO customize the update page here
      // send the html with "text/html"
      server.send(200, "text/plain", "Update completed!");
    } else {
      Serial.printf("Written only %d/%d bytes.\n", written, length);
    }

    // Check for success
    if (Update.end()) {
      if (Update.isFinished()) {
        Serial.println("OTA update successful. Restarting...");
        ESP.restart();
      } else {
        Serial.println("OTA update not finished.");
      }
    } else {
      Serial.printf("OTA update failed: %s\n", Update.errorString());
    }
  } else {
    Serial.printf("HTTP request failed. HTTP code: %d\n", httpCode);
  }

  http.end();
}

void setup() {
  Serial.begin(9600); // Debug serial
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // Modbus serial, 
  //SERIAL_8N1: configuration for serial communication. 8: 8 data bits, N: No parity bit, 1: 1 stop bit
  delay(500);  //Just to give the serial ports some time to initialise.

  pinMode(MAX485_DE, OUTPUT);
  pinMode(MAX485_RE_NEG, OUTPUT);
  digitalWrite(MAX485_DE, LOW); //Disables the RS485 driver.
  digitalWrite(MAX485_RE_NEG, LOW); // Enables the RS485 receiver.

  // Debugging information
  Serial.println("Setup complete. Starting communication...");

  // Initialize WiFi
  initWiFi();

  // Initialize NVS
  initNvs();  

  //Program will not continue unless WiFi is connected..
  initServer();
  //Detect number of meters and set global variable, numberOfMeters.
  detectNumberOfMeters();

  //Do the initial reading of the meters and update of the webpage, then repeat after 3 seconds in the loop.
  for (int i = 1; i <= numberOfMeters; i++) {
    handlePowerMeter(i); //Pass the meter number to the function.
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
      handlePowerMeter(i); //Pass the meter number to the function.
    }
    handleWebSocket();
    counter1 = now;
  }

  // Post meter data to remote server every 5 minutes
  if (now - counter2 > 300000) {
    //Post meter data to remote server
    for (int i = 1; i <= numberOfMeters; i++) {
      postToRemoteServer(i);
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


