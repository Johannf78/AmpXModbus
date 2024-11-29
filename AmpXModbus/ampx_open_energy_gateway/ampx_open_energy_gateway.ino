/*
the file "ampx_modbus_functions.ino" should be in the same directory as this .ino file.
It is automatically included and merged with this file.
This just seperates all the modbus functions and make this file easier to read.
*/

//This library is required to use digital pins as a software serial port.
//EspSoftwareSerial - Download and include this libarary in the library manager.
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <WebServer.h>
//Search for Arduino Websockets, install the one by Markus Sattler
#include <WebSocketsServer.h>
#include "webpage.h"
#include <ArduinoJson.h>

JsonDocument doc;

// Define the RS485 control pins
#define MAX485_DE 4
#define MAX485_RE_NEG 4
#define RX_PIN 16
#define TX_PIN 17
#define HTTP 80

SoftwareSerial modbusSerial(RX_PIN, TX_PIN);
const char* ssid = "FRITZ!Family";
const char* password = "03368098169909319946";

float voltage_on_L1 = 0.f;     // Voltage on L1
float voltage_on_L2 = 0.f;     // Voltage on L1
float voltage_on_L3 = 0.f;     // Voltage on L1
String m1_serial_number = "";  // Meter one serial number

WebServer server(HTTP);
WebSocketsServer webSocket = WebSocketsServer(81);

void processRegisters(uint16_t* results, uint16_t numRegisters,
                      const String& friendlyLabel, const String& docLabel) {
  for (uint16_t i = 0; i < numRegisters; i += 2) {
    /*
    Serial.print(label);
    Serial.print(" Register ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(results[i]);
    Serial.print(label);
    Serial.print(" Register ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(results[i + 1]);
    */
    uint32_t combinedValue = combineAndSwap(results[i], results[i + 1]);

    float value = convertToFloat(combinedValue);
    doc[docLabel] = String(value, 2);

    Serial.print(friendlyLabel);
    Serial.print(": ");
    Serial.println(value);
  }
}

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

  doc[docLabel] = String((float)value / 1000, 2);

  Serial.print(friendlyLabel);
  Serial.print(": ");
  Serial.println(value);
}

void initWiFi() {
  // Connect to the Wi-Fi network
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Now connecting to WiFi...");
  }
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void initServer() {
  server.on("/", handleRoot);
  server.begin();
  //Initialise the websockets on port 81
  webSocket.begin();
}

void handleRoot() {
  //String html = "<h1>AmpX Open Energy Gateway</h1>";
  //String vol = "<h1>Voltage on L1: " + String(voltage_on_L1, 2) + "(V)</h1>";
  //html += vol;
  //the String webpage has been defined in the included file webpage.h

  webpage.replace("m1_serial_number", m1_serial_number);
  server.send(200, "text/html", webpage);
}

void handlePowerMeter() {
  //These variable are populated from the data read on Modbus, they are reused for different parameters, voltage, current, power, etc.
  // uint16_t results[32];
  uint16_t responseBuffer[4];
  float value;

  // Read Serial number registers 70 and 71
  if (readHoldingRegisters(1, 70, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
                                                         /*
      Serial.print("Register 70: ");
      Serial.println(responseBuffer[0]);
      Serial.print("Register 71: ");
      Serial.println(responseBuffer[1]);
      */
    uint32_t combinedValue = combineAndSwap(responseBuffer[0], responseBuffer[1]);
    Serial.print("Serial Number: ");
    Serial.println(combinedValue);
    m1_serial_number = combinedValue;
  } else {
    Serial.println("Error reading registers 70 and 71");
  }

  // Read voltage on L1
  if (readHoldingRegisters(1, 1010, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Voltage L1", "voltage_L1");
  } else {
    Serial.println("Error reading voltage registers");
  }
  // Read voltage on L2
  if (readHoldingRegisters(1, 1012, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Voltage L2", "voltage_L2");
  } else {
    Serial.println("Error reading voltage registers");
  }
  // Read voltage on L3
  if (readHoldingRegisters(1, 1014, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Voltage L3", "voltage_L3");
  } else {
    Serial.println("Error reading voltage registers");
  }


  // Read current on L1
  if (readHoldingRegisters(1, 1000, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Current L1", "current_L1");
  } else {
    Serial.println("Error reading current registers");
  }
  // Read current on L2
  if (readHoldingRegisters(1, 1002, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Current L2", "current_L2");
  } else {
    Serial.println("Error reading current registers");
  }
  // Read current on L3
  if (readHoldingRegisters(1, 1004, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Current L3", "current_L3");
  } else {
    Serial.println("Error reading current registers");
  }
  // Read current average
  if (readHoldingRegisters(1, 1006, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Current Avg", "current_avg");
  } else {
    Serial.println("Error reading current registers");
  }



  // Read Active Power on L1
  if (readHoldingRegisters(1, 1028, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Active Power L1", "active_power_L1");
  } else {
    Serial.println("Error reading Active Power registers");
  }
  // Read Active Power on L2
  if (readHoldingRegisters(1, 1030, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Active Power L2", "active_power_L2");
  } else {
    Serial.println("Error reading Active Power registers");
  }
  // Read Power on L3
  if (readHoldingRegisters(1, 1032, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Active Power L3", "active_power_L3");
  } else {
    Serial.println("Error reading Active Power registers");
  }
  // Read Total Power
  if (readHoldingRegisters(1, 1034, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Active Power Total", "active_power_tot");
  } else {
    Serial.println("Error reading Active Power registers");
  }

  //Read Active Energy Imported L1
  if (readHoldingRegisters64(1, 2500, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, "Energy Imported L1", "active_energy_imported_L1");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  //Read Active Energy Imported L2
  if (readHoldingRegisters64(1, 2504, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, "Energy Imported L2", "active_energy_imported_L2");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  //Read Active Energy Imported L3
  if (readHoldingRegisters64(1, 2508, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, "Energy Imported L3", "active_energy_imported_L3");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  //Read Active Energy Imported Total
  if (readHoldingRegisters64(1, 2512, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, "Energy Imported Total", "active_energy_imported_tot");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
}

void handleWebSocket() {
  String str;

  serializeJson(doc, str);
  webSocket.broadcastTXT(str);
}

void setup() {
  Serial.begin(9600);
  modbusSerial.begin(9600);
  delay(500);  //Just to give the serial ports some time to initialise.
  pinMode(MAX485_DE, OUTPUT);
  pinMode(MAX485_RE_NEG, OUTPUT);
  digitalWrite(MAX485_DE, LOW);
  digitalWrite(MAX485_RE_NEG, LOW);

  // Debugging information
  Serial.println("Setup complete. Starting communication...");

  // Initialize WiFi
  initWiFi();
  //Program will not continuen unless WiFi is connected..
  initServer();
}

void loop() {
  static unsigned long previousNow = 0;
  unsigned long now = millis();

  // Read the parameters every 3 seconds
  if (now - previousNow > 3000) {
    handlePowerMeter();
    handleWebSocket();
    previousNow = now;
  }

  //These functions must run continuesly, so one can not include a delay in the main loop.
  server.handleClient();  //Handle webserver requests from client
  webSocket.loop();
}
