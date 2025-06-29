//Begin Shared Modbus functions between RS485 and TCPIP
float combineRegistersToFloat(uint16_t reg0, uint16_t reg1) {
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

uint64_t combineRegistersToInt64(uint16_t reg0, uint16_t reg1, uint16_t reg2, uint16_t reg3) {
  uint64_t combined = 0;
  // Combine in reverse order for little endian
  combined |= (uint64_t)reg3;
  combined |= (uint64_t)reg2 << 16;
  combined |= (uint64_t)reg1 << 32;
  combined |= (uint64_t)reg0 << 48;
  return combined;
} 

void processRegisters(uint16_t* registerData, uint16_t numRegisters, int registerDataType,
                      const String& friendlyLabel, const String& docLabel) {

  //Serial.println("");
  //Serial.println(friendlyLabel);  
  for (uint16_t i = 0; i < numRegisters; i++) {
    //Serial.print("registerData " + String(i) + ": ");
    //Serial.println(registerData[i]);
  }
  
  float floatValue = 0;
  uint32_t int32Value = 0;
  uint64_t int64Value = 0;
  String stringValue = "";

  if (registerDataType == dataTypeInt32){
    //Serial.println("dataTypeInt32");
    int32Value = combineRegistersToInt32(registerData[0], registerData[1]);
    stringValue = String(int32Value); //convert integer 32 to string
  }
  if (registerDataType == dataTypeInt64){
    //Serial.println("dataTypeInt64");
    int64Value = combineRegistersToInt64(registerData[0], registerData[1], registerData[2], registerData[3]);
    stringValue = String((float)int64Value / 1000, 2); //convert integer 64 to string
  }
  if (registerDataType == dataTypeFloat){
    //Serial.println("dataTypeFloat");
    floatValue = combineRegistersToFloat(registerData[0], registerData[1]);
    stringValue = String(floatValue,2); //convert float to string with two decimal places
  }

  //Update the json document with the value
  JsonDoc[docLabel] = stringValue;

  //Serial.print("docLabel: " + docLabel + ", ");
  //Serial.print(friendlyLabel);
  //Serial.print(": ");
  //Serial.println(stringValue);
  
}
//End Shared Modbus functions between RS485 and TCPIP