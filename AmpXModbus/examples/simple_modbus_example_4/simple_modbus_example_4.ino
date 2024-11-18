/*
Read 64-bit energy register from a Modbus slave device, using ModbusMaster library.
*/
#include <ModbusMaster.h>
#include <SoftwareSerial.h>

// Define the RS485 control pins
#define MAX485_DE 4
#define MAX485_RE_NEG 4
#define RX_PIN 16
#define TX_PIN 17

SoftwareSerial modbusSerial(RX_PIN, TX_PIN);
ModbusMaster node;

// Pre-transmission callback
void preTransmission() {
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
}

// Post-transmission callback
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

void setup() {
  Serial.begin(9600);
  modbusSerial.begin(9600);
  
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
  
  node.begin(1, modbusSerial); // Slave ID 1
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  
  Serial.println("Starting..");
}

void loop() {
  uint8_t result;
  uint16_t responseBuffer[4];
  
    delay(1000);

  result = node.readHoldingRegisters(2500, 4);
  
  if (result == node.ku8MBSuccess) {
    for (uint8_t i = 0; i < 4; i++) {
      responseBuffer[i] = node.getResponseBuffer(i);
      Serial.print("Energy Register ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(responseBuffer[i]);
    }

    // Print the values in hexadecimal format
    Serial.print("Energy Register 0 (hex): 0x");
    Serial.println(responseBuffer[0], HEX);
    Serial.print("Energy Register 1 (hex): 0x");
    Serial.println(responseBuffer[1], HEX);
    Serial.print("Energy Register 2 (hex): 0x");
    Serial.println(responseBuffer[2], HEX);
    Serial.print("Energy Register 3 (hex): 0x");
    Serial.println(responseBuffer[3], HEX);

    uint64_t energy = combineAndSwap64(responseBuffer[0], responseBuffer[1], responseBuffer[2], responseBuffer[3]);
    Serial.print("Energy: ");
    Serial.println(energy);

    Serial.print("Energy (hex): 0x");
    Serial.println(energy, HEX);
    
  } else {
    Serial.print("Failed to read registers. Error: ");
    Serial.println(result);
  }
  
  delay(1000);
}