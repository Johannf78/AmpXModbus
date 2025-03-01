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

void processRegisters64(uint16_t *results, uint16_t numRegisters, const char* label) {
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

    //uint32_t combinedValue = combineAndSwap(results[i], results[i + 1]);
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
  uint16_t results[32];

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

  // Read voltage on L1, L2, L3
  if (readHoldingRegisters(1, 1010, 6, results)) { // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(results, 6, "Voltage");
  } else {
    Serial.println("Error reading voltage registers");
  }

  // Read current on L1, L2, L3, and average current
  if (readHoldingRegisters(1, 1000, 8, results)) { // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(results, 8, "Current");
  } else {
    Serial.println("Error reading current registers");
  }


  // Read Power on L1, L2, L3
  if (readHoldingRegisters(1, 1036, 6, results)) { // 1 is the Modbus slave ID, adjust if necessary
    processRegisters(results, 6, "Power");
  } else {
    Serial.println("Error reading Power registers");
  }

  if (readHoldingRegisters(1, 2500, 32, results)) { // 1 is the Modbus slave ID, adjust if necessary
    processRegisters64(results, 32, "energy");
  } else {
    Serial.println("Error reading energy registers");
  }

  
  delay(5000); // Wait for a second before the next read
}