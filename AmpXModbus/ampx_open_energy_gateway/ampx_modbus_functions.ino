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
  Serial2.write(request, len);
  Serial2.flush();
  postTransmission();
}

bool receiveResponse(uint8_t *response, uint8_t len) {
  uint32_t startTime = millis();
  uint8_t index = 0;
  while (millis() - startTime < 1000) {  // 1 second timeout
    if (Serial2.available()) {
      response[index++] = Serial2.read();
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


float combineRegistersToFloat(uint16_t reg0, uint16_t reg1) {
/*
Combine the two 16-bit values into a 32-bit value in little endian order.
Convert the combined 32-bit value to a float.
Return the float value.
*/

    // Combine the two 16-bit values into a 32-bit value in little endian order
    uint32_t combined = ((uint32_t)reg0 << 16) | reg1;
    
    // Convert the combined 32-bit value to a float
    float result;
    memcpy(&result, &combined, sizeof(result));
    
    return result;
}


uint32_t combineRegistersToInt32(uint16_t reg0, uint16_t reg1) {
  // Combine the two 16-bit values into a 32-bit value in little endian order
  uint32_t combined = ((uint32_t)reg0 << 16) | reg1;
  return combined;
}


//Old combineAndSwap64
uint64_t combineRegistersToInt64(uint16_t reg0, uint16_t reg1, uint16_t reg2, uint16_t reg3) {
  uint64_t combined = 0;
  // Combine in reverse order for little endian
  combined |= (uint64_t)reg3;
  combined |= (uint64_t)reg2 << 16;
  combined |= (uint64_t)reg1 << 32;
  combined |= (uint64_t)reg0 << 48;
  return combined;
}
/*
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
  Serial2.write(request, 8);
  Serial2.flush();
  postTransmission();

  uint8_t response[5 + 2 * numRegisters];
  uint32_t startTime = millis();
  uint8_t index = 0;

  while (millis() - startTime < 1000) {
    if (Serial2.available()) {
      response[index++] = Serial2.read();
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
*/