#include <SoftwareSerial.h>

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

uint64_t combineAndSwap64(uint16_t reg0, uint16_t reg1, uint16_t reg2, uint16_t reg3) {
  uint64_t combined = 0;
  // Combine in reverse order for little endian
  combined |= (uint64_t)reg3;
  combined |= (uint64_t)reg2 << 16;
  combined |= (uint64_t)reg1 << 32;
  combined |= (uint64_t)reg0 << 48;
  return combined;
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

bool readHoldingRegisters(uint8_t slaveID, uint16_t startAddress, uint16_t numRegisters, uint16_t *responseBuffer) {
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
        if (lowByte(crc) == response[index-2] && highByte(crc) == response[index-1]) {
          for (uint8_t i = 0; i < numRegisters; i++) {
            responseBuffer[i] = (response[3 + 2*i] << 8) | response[4 + 2*i];
          }
          return true;
        }
        break;
      }
    }
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
    // Print registers in hex
    Serial.print("Registers (hex): ");
    for(int i = 0; i < 4; i++) {
      Serial.print("0x");
      Serial.print(responseBuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    uint64_t energy = combineAndSwap64(responseBuffer[0], responseBuffer[1], 
                                     responseBuffer[2], responseBuffer[3]);
    
    Serial.print("Energy: ");
    Serial.println(energy);
    Serial.print("Energy (hex): 0x");
    Serial.println((unsigned long)energy, HEX);
  }
  
  delay(1000);
}