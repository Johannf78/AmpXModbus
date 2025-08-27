void initNvs() {
  if (preferences.begin("app", false)) {
    Serial.println("NVS initialized successfully!");
  } else {
    Serial.println("Failed to initialize NVS.");
  }
  Serial.println("");
}



void handleChangeMetersName() {
  String m1_name = server.arg("m1_name");
  preferences.putString("m1_name", m1_name);
  Serial.println("m1_name: " + m1_name);

  String m2_name = server.arg("m2_name");
  preferences.putString("m2_name", m2_name);
  Serial.println("m2_name: " + m2_name);

  String m3_name = server.arg("m3_name");
  preferences.putString("m3_name", m3_name);
  Serial.println("m3_name: " + m3_name);

  String m4_name = server.arg("m4_name");
  preferences.putString("m4_name", m4_name);
  Serial.println("m4_name: " + m4_name);


  server.send(200, "text/html", "Updated successfully. <br>Back to:<a href='/'>Home</a> | <a href='/settings'>Settings</a>");
}


//Detect the number of meters connected to the gateway.
void detectNumberOfMeters(){
  debugln("detectNumberOfMeters...");
  debug("MODBUS_TYPE" );
  debugln(MODBUS_TYPE);

  #if MODBUS_TYPE == MODBUS_TYPE_RS485
  
    uint16_t registerData[4];
    //Find number of meters, 4 max number for now.
    for (int i = 1; i <= maxNumberOfMeters; i++) {
      // Read Serial number registers 70 and 71
      if (modbus_read_registers_rs485(i, 70, 2, registerData)) {  // i is the Modbus slave ID
        uint32_t combinedValue = combineRegistersToInt32(registerData[0], registerData[1]);
        //Serial.print("Serial Number: ");
        //Serial.println(combinedValue);
        //Update the number of meters if able to read its serial number
        numberOfMeters = i;    
      } else {
      Serial.println("Error reading meter: " + String(i));
      }
    } 

  #else
  
    // MODBUS_TYPE = MODBUS_TYPE_TCPIP
    // TCPIP is only used for handheld meters, there will always just be one.
    numberOfMeters = 1;
  
  #endif
  
  Serial.println("Number of meters detected: " + String(numberOfMeters)); 
  if (numberOfMeters >= 1){
    //Indicate that one or more meters were detected by turning on a LED.
    digitalWrite(LED_2_METER, HIGH);
  }
  Serial.println("");
}


//JF New combined function for RS485 and TCP
void handlePowerMeter(int meterNumber = 1) {
  uint16_t registerData[4];
  String meterPrefix = "m" + String(meterNumber) + "_";
  
  // Loop through all register definitions in the JSON document
  for (JsonPair kv : MeterRegisterDefs.as<JsonObject>()) {
    JsonArray registerDef = kv.value().as<JsonArray>();
    
    int registerNumber = registerDef[0];
    int numRegisters = registerDef[1];
    int dataType = registerDef[2];
    String friendlyName = registerDef[3];
    String jsonKey = registerDef[4];
    bool read_success;


    #if MODBUS_TYPE == MODBUS_TYPE_RS485
      if (modbus_read_registers_rs485(meterNumber, registerNumber, numRegisters, registerData)) {
        read_success=1;
      }
    #else
      if(modbus_read_registers_tcpip(registerNumber, numRegisters, registerData)){
        read_success=1;
      }
    #endif
  

    if (read_success){
      // Special handling for serial number
      if (registerNumber == 70) {
        uint32_t serialNum = combineRegistersToInt32(registerData[0], registerData[1]);
        if (meterNumber == 1) m1_serial_number = String(serialNum);
        else if (meterNumber == 2) m2_serial_number = String(serialNum);
        else if (meterNumber == 3) m3_serial_number = String(serialNum);
        else if (meterNumber == 4) m4_serial_number = String(serialNum);
      } else {
        // Normal handling for other registers
        processRegisters(registerData, numRegisters, dataType, friendlyName, meterPrefix + jsonKey);
      
        // Special case for energy imported total, add a duplicate for summary field
        if (registerNumber == 2512) {
          processRegisters(registerData, numRegisters, dataType, friendlyName, meterPrefix + jsonKey + "_summary");
        }
      }
    } else {
      Serial.println("Error reading register " + String(registerNumber));
    }
  }

  // Display the JSON Document for Meter
  Serial.println("=== JSON Document for Meter " + String(meterNumber) + " ===");
  String jsonString;
  serializeJsonPretty(JsonDoc, jsonString);
  Serial.println(jsonString);
  Serial.println("=== JSON Size: " + String(JsonDoc.memoryUsage()) + " bytes ===");
  Serial.println();
  
}

