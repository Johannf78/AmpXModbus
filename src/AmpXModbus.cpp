// AmpXModbusLibrary.cpp
#include "AmpXModbus.h"

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