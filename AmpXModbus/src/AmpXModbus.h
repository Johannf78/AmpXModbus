// AmpXModbusLibrary.h
#ifndef AMPXMODBUSLIBRARY_H
#define AMPXMODBUSLIBRARY_H

#include <Arduino.h>

uint16_t calculateCRC(uint8_t *data, uint8_t length);
//uint16_t combineAndSwap(uint8_t high, uint8_t low);


#endif // AMPXMODBUSLIBRARY_H