ise// AmpXModbusLibrary.h
#ifndef AMPXMODBUSLIBRARY_H
#define AMPXMODBUSLIBRARY_H

#include <Arduino.h>

uint16_t calculateCRC(uint8_t *data, uint8_t length);
float convertToFloat(uint32_t value);
uint32_t combineAndSwap(uint16_t highWord, uint16_t lowWord);

#endif // AMPXMODBUSLIBRARY_H