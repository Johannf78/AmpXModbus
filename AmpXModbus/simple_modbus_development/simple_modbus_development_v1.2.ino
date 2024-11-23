
//This library is required to use digital pins as a software serial port.
#include <SoftwareSerial.h>
//EspSoftwareSerial - Download and include this libarary.
#include <WiFi.h>
#include <WebServer.h>

//Move the libary to the default location, or change the path to the correct location.
//#include "D:\\OneDrive\\JF Data\\UserData\\Documents\\Arduino\\libraries\\AmpXModbusLibrary\\src\\AmpXModbus.h"
//#include "D:\\OneDrive\\Dev\\Ardruino\\AmpX\\ESP32\\AmpXModbus\\AmpXModbus\\src\\AmpXModbus.h"
//#include <AmpXModbus.h>

// Define the RS485 control pins
#define MAX485_DE 4
#define MAX485_RE_NEG 4
#define RX_PIN 16
#define TX_PIN 17
#define HTTP 80

SoftwareSerial modbusSerial(RX_PIN, TX_PIN);
const char *ssid = "FRITZ!Family";
const char *password = "03368098169909319946";

float voltage_on_L1 = 0.f; // Voltage on L1

WebServer server(HTTP);

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
}

void postTransmission() {
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
}

uint16_t calculateCRC(uint8_t *buffer, uint8_t len) {
  uint16_t crc = 0xFFFF;
  for (uint8_t i = 0; i < len; i++) {
    crc ^= buffer[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

void sendRequest(uint8_t *request, uint8_t len) {
  preTransmission();
  modbusSerial.write(request, len);
  modbusSerial.flush();
  postTransmission();
}

bool receiveResponse(uint8_t *response, uint8_t len) {
  uint32_t startTime = millis();
  uint8_t index = 0;
  while (millis() - startTime < 1000) {  // 1 second timeout
    if (modbusSerial.available()) {
      response[index++] = modbusSerial.read();
      if (index == len) {
        uint16_t crc = calculateCRC(response, len - 2);
        if (lowByte(crc) == response[len - 2] && highByte(crc) == response[len - 1]) {
          return true;
        } else {
          Serial.println("CRC error");
        }
        break;
      }
    }
  }
  Serial.println("Timeout or incomplete response");
  return false;
}

bool readHoldingRegisters(uint8_t slaveID, uint16_t startAddress, uint16_t numRegisters, uint16_t *responseBuffer) {
  uint8_t request[8];
  request[0] = slaveID;
  request[1] = 0x03;  // Function code for reading holding registers
  request[2] = highByte(startAddress);
  request[3] = lowByte(startAddress);
  request[4] = highByte(numRegisters);
  request[5] = lowByte(numRegisters);
  uint16_t crc = calculateCRC(request, 6);
  request[6] = lowByte(crc);
  request[7] = highByte(crc);

  /*
  Serial.print("Sending request: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(request[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  */
  sendRequest(request, 8);

  uint8_t response[5 + 2 * numRegisters];
  if (receiveResponse(response, 5 + 2 * numRegisters)) {
    for (uint16_t i = 0; i < numRegisters; i++) {
      responseBuffer[i] = word(response[3 + 2 * i], response[4 + 2 * i]);
    }
    return true;
  } else {
    Serial.print("Received response: ");
    for (int i = 0; i < 5 + 2 * numRegisters; i++) {
      Serial.print(response[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  return false;
}

void processRegisters(uint16_t *results, uint16_t numRegisters, const char *label, float* v = NULL) {
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
    if (v != NULL)
      v[i / 2] = value;
    Serial.print(label);
    Serial.print(": ");
    Serial.println(value);
  }
}

float convertToFloat(uint32_t value) {
  float result;
  uint8_t *valuePtr = (uint8_t *)&value;
  uint8_t swapped[4];
  swapped[0] = valuePtr[3];
  swapped[1] = valuePtr[2];
  swapped[2] = valuePtr[1];
  swapped[3] = valuePtr[0];
  memcpy(&result, swapped, sizeof(result));
  return result;
}

uint32_t combineAndSwap(uint16_t highWord, uint16_t lowWord) {
  uint32_t combined = ((uint32_t)highWord << 16) | lowWord;
  return ((combined & 0xFF000000) >> 24) | ((combined & 0x00FF0000) >> 8) | ((combined & 0x0000FF00) << 8) | ((combined & 0x000000FF) << 24);
}

uint64_t combineAndSwap64(uint16_t reg0, uint16_t reg1, uint16_t reg2, uint16_t reg3) {
  uint64_t combined = 0;
  // Combine in reverse order for little endian
  combined |= (uint64_t)reg3;
  combined |= (uint64_t)reg2 << 16;
  combined |= (uint64_t)reg1 << 32;
  combined |= (uint64_t)reg0 << 48;
  return combined;
}

bool readHoldingRegisters64(uint8_t slaveID, uint16_t startAddress, uint16_t numRegisters, uint16_t *responseBuffer) {
  uint8_t request[8];
  request[0] = slaveID;
  request[1] = 0x03;
  request[2] = highByte(startAddress);
  request[3] = lowByte(startAddress);
  request[4] = highByte(numRegisters);
  request[5] = lowByte(numRegisters);
  uint16_t crc = calculateCRC(request, 6);
  request[6] = lowByte(crc);
  request[7] = highByte(crc);

  preTransmission();
  modbusSerial.write(request, 8);
  modbusSerial.flush();
  postTransmission();

  uint8_t response[5 + 2 * numRegisters];
  uint32_t startTime = millis();
  uint8_t index = 0;

  while (millis() - startTime < 1000) {
    if (modbusSerial.available()) {
      response[index++] = modbusSerial.read();
      if (index == sizeof(response)) {
        uint16_t crc = calculateCRC(response, index - 2);
        if (lowByte(crc) == response[index - 2] && highByte(crc) == response[index - 1]) {
          for (uint8_t i = 0; i < numRegisters; i++) {
            responseBuffer[i] = (response[3 + 2 * i] << 8) | response[4 + 2 * i];
          }
          return true;
        }
        break;
      }
    }
  }
  return false;
}

void processRegistersInt64(uint16_t *responseBuffer, uint16_t numRegisters, const char *label) {
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
  Serial.print(label);
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
}

void handleRoot() {
  String vol = "<h1>Voltage on L1: " + String(voltage_on_L1, 2) + "(V)</h1>";
  server.send(200, "text/html", vol);
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
  initServer();
}

void handlePowerMeter() {
  //These variable are populated from the data read on Modbus, they are reused for different parameters, voltage, current, power, etc.
  // uint16_t results[32];
  uint16_t responseBuffer[4];

  // Read Serial number registers 70 and 71
  if (readHoldingRegisters(1, 0x46, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
                                                           /*
      Serial.print("Register 70: ");
      Serial.println(results[0]);
      Serial.print("Register 71: ");
      Serial.println(results[1]);
      */
    uint32_t combinedValue = combineAndSwap(responseBuffer[0], responseBuffer[1]);
    Serial.print("Serial Number: ");
    Serial.println(combinedValue);
  } else {
    Serial.println("Error reading registers 70 and 71");
  }

  // Read voltage on L1
  if (readHoldingRegisters(1, 1010, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Voltage L1", &voltage_on_L1);
  } else {
    Serial.println("Error reading voltage registers");
  }
  // Read voltage on L2
  if (readHoldingRegisters(1, 1012, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Voltage L2");
  } else {
    Serial.println("Error reading voltage registers");
  }
  // Read voltage on L3
  if (readHoldingRegisters(1, 1014, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Voltage L3");
  } else {
    Serial.println("Error reading voltage registers");
  }


  // Read current on L1
  if (readHoldingRegisters(1, 1000, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Current L1");
  } else {
    Serial.println("Error reading current registers");
  }
  // Read current on L2
  if (readHoldingRegisters(1, 1002, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Current L2");
  } else {
    Serial.println("Error reading current registers");
  }
  // Read current on L3
  if (readHoldingRegisters(1, 1004, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Current L3");
  } else {
    Serial.println("Error reading current registers");
  }
  // Read current average
  if (readHoldingRegisters(1, 1006, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Current Avg");
  } else {
    Serial.println("Error reading current registers");
  }



  // Read Active Power on L1
  if (readHoldingRegisters(1, 1028, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Active Power L1");
  } else {
    Serial.println("Error reading Active Power registers");
  }
  // Read Active Power on L2
  if (readHoldingRegisters(1, 1030, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Active Power L2");
  } else {
    Serial.println("Error reading Active Power registers");
  }
  // Read Power on L3
  if (readHoldingRegisters(1, 1032, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Active Power L3");
  } else {
    Serial.println("Error reading Active Power registers");
  }
  // Read Total Power
  if (readHoldingRegisters(1, 1034, 2, responseBuffer)) {  // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(responseBuffer, 2, "Active Power Total");
  } else {
    Serial.println("Error reading Active Power registers");
  }

  //Read Active Energy Imported L1
  if (readHoldingRegisters64(1, 2500, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, "Energy Imported L1");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  //Read Active Energy Imported L2
  if (readHoldingRegisters64(1, 2504, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, "Energy Imported L2");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  //Read Active Energy Imported L3
  if (readHoldingRegisters64(1, 2508, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, "Energy Imported L3");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
  //Read Active Energy Imported Total
  if (readHoldingRegisters64(1, 2512, 4, responseBuffer)) {
    processRegistersInt64(responseBuffer, 4, "Energy Imported Total");
  } else {
    Serial.println("Error reading Active Energy registers");
  }
}

void loop() {
  static unsigned long t_ = 0;

  unsigned long t = millis();

  // Read the parameters every 3 seconds
  if (t - t_ > 3000) {
    handlePowerMeter();
    t_ = t;
  }

  server.handleClient();
}
