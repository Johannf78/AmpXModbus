#include <SoftwareSerial.h>

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

uint64_t combineAndSwap64(uint16_t reg0, uint16_t reg1, uint16_t reg2, uint16_t reg3) {
  uint64_t combined = 0;
  combined |= (uint64_t)reg3 << 48;
  combined |= (uint64_t)reg2 << 32;
  combined |= (uint64_t)reg1 << 16;
  combined |= (uint64_t)reg0;
  return combined;
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

  sendRequest(request, 8);

  uint8_t response[5 + 2 * numRegisters];
  if (receiveResponse(response, sizeof(response))) {
    for (uint8_t i = 0; i < numRegisters; i++) {
      responseBuffer[i] = (response[3 + 2 * i] << 8) | response[4 + 2 * i];
    }
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  delay(10000);
  Serial.println("Starting..");

  modbusSerial.begin(9600);
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
}

void loop() {
  uint16_t responseBuffer[4];
  if (readHoldingRegisters(1, 2500, 4, responseBuffer)) {
    Serial.print("Energy Register 0: ");
    Serial.println(responseBuffer[0]);
    Serial.print("Energy Register 1: ");
    Serial.println(responseBuffer[1]);
    Serial.print("Energy Register 2: ");
    Serial.println(responseBuffer[2]);
    Serial.print("Energy Register 3: ");
    Serial.println(responseBuffer[3]);

    uint64_t energy = combineAndSwap64(responseBuffer[0], responseBuffer[1], responseBuffer[2], responseBuffer[3]);
    Serial.print("Energy: ");
    Serial.println(energy);
  } else {
    Serial.println("Failed to read registers");
  }

  delay(1000); // 1 second delay
}