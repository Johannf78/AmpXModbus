//This library is required to use digital pins as a software serial port.
#include <SoftwareSerial.h>
//EspSoftwareSerial - Include this libarary.

//Move the libary to the default location, or change the path to the correct location.
//#include "D:\\OneDrive\\JF Data\\UserData\\Documents\\Arduino\\libraries\\AmpXModbusLibrary\\src\\AmpXModbus.h"
//#include "D:\\OneDrive\\Dev\\Ardruino\\AmpX\\ESP32\\AmpXModbus\\AmpXModbus\\src\\AmpXModbus.h"
//#include <AmpXModbus.h>

// Define the RS485 control pins
#define MAX485_DE 4
#define MAX485_RE_NEG 4
#define RX_PIN 16
#define TX_PIN 17

SoftwareSerial modbusSerial(RX_PIN, TX_PIN);

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

void processRegisters(uint16_t *results, uint16_t numRegisters, const char* label) {
  for (uint16_t i = 0; i < numRegisters; i += 2) {
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
    uint32_t combinedValue = combineAndSwap(results[i], results[i + 1]);
    float value = convertToFloat(combinedValue);
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
  return ((combined & 0xFF000000) >> 24) |
         ((combined & 0x00FF0000) >> 8)  |
         ((combined & 0x0000FF00) << 8)  |
         ((combined & 0x000000FF) << 24);
}

uint64_t combineAndSwap64(uint16_t word1, uint16_t word2, uint16_t word3, uint16_t word4) {
  uint64_t combined = ((uint64_t)word1 << 48) |
                      ((uint64_t)word2 << 32) |
                      ((uint64_t)word3 << 16) |
                      (uint64_t)word4;

  Serial.print("Combined: ");
  Serial.println(combined, HEX);

  uint64_t swapped = ((combined & 0xFF00000000000000) >> 56) |
                     ((combined & 0x00FF000000000000) >> 40) |
                     ((combined & 0x0000FF0000000000) >> 24) |
                     ((combined & 0x000000FF00000000) >> 8)  |
                     ((combined & 0x00000000FF000000) << 8)  |
                     ((combined & 0x0000000000FF0000) << 24) |
                     ((combined & 0x000000000000FF00) << 40) |
                     ((combined & 0x00000000000000FF) << 56);

  Serial.print("Swapped: ");
  Serial.println(swapped, HEX);

  return swapped;
}

bool readInt64Registers(uint8_t slaveID, uint16_t startAddress, uint16_t numRegisters, int64_t *responseBuffer) {
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
    for (uint16_t i = 0; i < numRegisters; i += 4) {
      responseBuffer[i / 4] = ((int64_t)word(response[3 + 2 * i], response[4 + 2 * i]) << 32) |
                              ((int64_t)word(response[3 + 2 * (i + 2)], response[4 + 2 * (i + 2)]));
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

void processInt64Registers(int64_t *results, uint16_t numRegisters, const char* label) {
  for (uint16_t i = 0; i < numRegisters; i += 4) {
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

    Serial.print(label);
    Serial.print(" Register ");
    Serial.print(i + 2);
    Serial.print(": ");
    Serial.println(results[i + 2]);

    Serial.print(label);
    Serial.print(" Register ");
    Serial.print(i + 3);
    Serial.print(": ");
    Serial.println(results[i + 3]);

    int64_t combinedValue = combineAndSwap64(results[i], results[i + 1], results[i + 2], results[i + 3]);
    //float value = convertToFloat(combinedValue);
    //Serial.print(label);
    //Serial.print(": ");
    //Serial.println(value);
  }
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
  //These variable are populated from the data read on Modbus, they are reused for different parameters, voltage, current, power, etc.
  uint16_t results[32];
  int64_t int64Results[2];
  /*
  // Read Serial number registers 70 and 71
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
*/
  // Read voltage on L1 //, L2, L3
  if (readHoldingRegisters(1, 1010, 2, results)) { // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(results, 2, "Voltage");
  } else {
    Serial.println("Error reading voltage registers");
  }

/*
  // Read current on L1, L2, L3, and average current
  if (readHoldingRegisters(1, 1000, 8, results)) { // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(results, 8, "Current");
  } else {
    Serial.println("Error reading current registers");
  }

  // Read Power on L1, L2, L3
  if (readHoldingRegisters(1, 1036, 6, results)) { // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(results, 6, "Power");
  }
  } else {
    Serial.println("Error reading Power registers");
  }
*/
  // Read energy registers from 2500 to 2528. 64 Bit integer, 4 bytes, little endian, 4 x 4 = 16 bytes, 2 Bytes per register 
  if (readInt64Registers(1, 2500, 4, int64Results)) { // 1 is the Modbus slave ID, adjust if necessary
    processInt64Registers(int64Results, 4, "Energy");
  } else {
    Serial.println("Error reading energy registers");
  }

  Serial.println(""); //Blank line to make it more readable.
  delay(10000); // Wait for a second before the next read
}