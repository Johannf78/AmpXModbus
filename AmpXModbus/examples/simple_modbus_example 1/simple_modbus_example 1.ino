#include <SoftwareSerial.h>
//#include <AmpXModbusLibrary.h>
//#include "D:\\OneDrive\\JF Data\\UserData\\Documents\\Arduino\\libraries\\AmpXModbusLibrary\\src\\AmpXModbusLibrary.h"
#include <AmpXModbus.h>


// Define the RS485 control pins
#define MAX485_DE 4
#define MAX485_RE_NEG 4
#define RX_PIN 16
#define TX_PIN 17

SoftwareSerial modbusSerial(RX_PIN, TX_PIN);

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
}

void postTransmission() {
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
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
  while (millis() - startTime < 1000) { // 1 second timeout
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
/*
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
*/
bool readHoldingRegisters(uint8_t slaveID, uint16_t startAddress, uint16_t numRegisters, uint16_t *responseBuffer) {
  uint8_t request[8];
  request[0] = slaveID;
  request[1] = 0x03; // Function code for reading holding registers
  request[2] = highByte(startAddress);
  request[3] = lowByte(startAddress);
  request[4] = highByte(numRegisters);
  request[5] = lowByte(numRegisters);
  uint16_t crc = calculateCRC(request, 6);
  request[6] = lowByte(crc);
  request[7] = highByte(crc);

  Serial.print("Sending request: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(request[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

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



void setup() {
  Serial.begin(9600);
  modbusSerial.begin(9600);
  pinMode(MAX485_DE, OUTPUT);
  pinMode(MAX485_RE_NEG, OUTPUT);
  digitalWrite(MAX485_DE, LOW);
  digitalWrite(MAX485_RE_NEG, LOW);

  // Debugging information
  Serial.println("Setup complete. Starting communication...");
}

void loop() {
  uint16_t results[8];

  // Read registers 70 and 71
  if (readHoldingRegisters(1, 0x46, 2, results)) { // 1 is the Modbus slave ID, adjust if necessary
    Serial.print("Register 70: ");
    Serial.println(results[0]);
    Serial.print("Register 71: ");
    Serial.println(results[1]);
    uint32_t combinedValue = combineAndSwap(results[0], results[1]);
    Serial.print("Combined and Swapped Value: ");
    Serial.println(combinedValue);

  } else {
    Serial.println("Error reading registers 70 and 71");
  }

  // Read voltage on L1 (registers 1010 and 1011)
  if (readHoldingRegisters(1, 1010, 6, results)) { // 1 is the Modbus slave ID, adjust if necessary
    Serial.print("Voltage Register 1010: ");
    Serial.println(results[0]);
    Serial.print("Voltage Register 1011: ");
    Serial.println(results[1]);
    uint32_t combinedVoltage = combineAndSwap(results[0], results[1]);
    float voltage = convertToFloat(combinedVoltage);
    Serial.print("Voltage on L1: ");
    Serial.println(voltage);

    Serial.print("Voltage Register 1012: ");
    Serial.println(results[2]);
    Serial.print("Voltage Register 1013: ");
    Serial.println(results[3]);
     combinedVoltage = combineAndSwap(results[2], results[3]);
     voltage = convertToFloat(combinedVoltage);
    Serial.print("Voltage on L2: ");
    Serial.println(voltage);

    Serial.print("Voltage Register 1014: ");
    Serial.println(results[4]);
    Serial.print("Voltage Register 1015: ");
    Serial.println(results[5]);
     combinedVoltage = combineAndSwap(results[4], results[5]);
     voltage = convertToFloat(combinedVoltage);
    Serial.print("Voltage on L3: ");
    Serial.println(voltage);

  } else {
    Serial.println("Error reading voltage registers");
  }

  // Read voltage on L1 (registers 1010 and 1011)
  if (readHoldingRegisters(1, 1000, 8, results)) { // 1 is the Modbus slave ID, adjust if necessary
    Serial.print("Current Register 1000: ");
    Serial.println(results[0]);
    Serial.print("Current Register 1001: ");
    Serial.println(results[1]);
    uint32_t combinedCurrent = combineAndSwap(results[0], results[1]);
    float current = convertToFloat(combinedCurrent);
    Serial.print("Current on L1: ");
    Serial.println(current);

    Serial.print("Current Register 1002: ");
    Serial.println(results[2]);
    Serial.print("Current Register 1003: ");
    Serial.println(results[3]);
     combinedCurrent = combineAndSwap(results[2], results[3]);
     current = convertToFloat(combinedCurrent);
    Serial.print("Current on L2: ");
    Serial.println(current);

    Serial.print("Current Register 1004: ");
    Serial.println(results[4]);
    Serial.print("Current Register 1005: ");
    Serial.println(results[5]);
     combinedCurrent = combineAndSwap(results[4], results[5]);
     current = convertToFloat(combinedCurrent);
    Serial.print("Current on L3: ");
    Serial.println(current);

    Serial.print("Current Register 1006: ");
    Serial.println(results[6]);
    Serial.print("Current Register 1007: ");
    Serial.println(results[7]);
     combinedCurrent = combineAndSwap(results[6], results[7]);
     current = convertToFloat(combinedCurrent);
    Serial.print("Average Current: ");
    Serial.println(current);


  } else {
    Serial.println("Error reading current registers");
  }


  delay(5000); // Wait for a second before the next read
}s