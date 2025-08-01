void initNvs() {
  if (preferences.begin("app", false)) {
    Serial.println("NVS initialized successfully!");
  } else {
    Serial.println("Failed to initialize NVS.");
  }
  Serial.println("");
}


void initNTP() {
  Serial.println("Initializing NTP time synchronization...");
  
  // Configure NTP servers and timezone
  // GMT offset in seconds (0 for UTC, 3600 for UTC+1, etc.)
  // Daylight saving offset in seconds (3600 for 1 hour, 0 for none)
  // You can adjust these based on your timezone
  const long gmtOffset_sec = 0;  // UTC time
  const int daylightOffset_sec = 0;  // No daylight saving
  
  // Configure time with NTP servers
  configTime(gmtOffset_sec, daylightOffset_sec, 
             "pool.ntp.org", "time.nist.gov", "time.google.com");
  
  // Wait for time synchronization
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  int timeout = 20; // 20 second timeout
  
  while (now < 24 * 3600 && timeout > 0) {  // Wait until we have a valid timestamp
    delay(1000);
    Serial.print(".");
    now = time(nullptr);
    timeout--;
  }
  
  if (timeout > 0) {
    Serial.println(" SUCCESS!");
    Serial.print("Current time: ");
    Serial.println(ctime(&now));
  } else {
    Serial.println(" TIMEOUT!");
    Serial.println("Failed to sync with NTP servers. Using system time.");
  }
}

String getCurrentTimestamp() {
  time_t now;
  struct tm timeinfo;
  
  // Get current time
  time(&now);
  
  // Convert to UTC time structure
  gmtime_r(&now, &timeinfo);
  
  // Format as ISO 8601 timestamp: YYYY-MM-DDTHH:MM:SSZ
  char timestamp[25];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  
  return String(timestamp);
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

    // 🔥 ADD THIS CODE TO DISPLAY THE JSON 🔥
  
  Serial.println("=== JSON Document for Meter " + String(meterNumber) + " ===");
  String jsonString;
  serializeJsonPretty(JsonDoc, jsonString);
  Serial.println(jsonString);
  Serial.println("=== JSON Size: " + String(JsonDoc.memoryUsage()) + " bytes ===");
  Serial.println();
  
}

/*
void postToAmpXPortal(int meterNumber = 1) { //old postToRemoteServer
  Serial.println("postToAmpXPortal function called");

  String meterPrefix = "m" + String(meterNumber) + "_";

    // Prepare JSON payload
  String httpRequestData = "{\"gateway_id\":\"" + String(AMPX_GATEWAY_ID) + 
                            "\",\"meter_id\":" + String(meterNumber) + 
                            ",\"energy_usage\":" + String(JsonDoc[meterPrefix + "active_energy_imported_tot"]) + 
                            ",\"power_max\":" + String(JsonDoc[meterPrefix + "active_power_tot"]) + "}";
  Serial.println("httpRequestData:  " + httpRequestData);

  //const char* ampxportal_apipath = "wp-json/ampx-energy/v1/log";
  String ampxportal_apipath = "wp-json/ampx-energy/v1/log";
  
  //String url = String(ampxportal_server_local) + String(ampxportal_apipath);
  String url = String(ampxportal_server_live) + String(ampxportal_apipath);
  Serial.println("URL: " + url);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;   
    http.begin(url);
    //Specify content-type header
    http.addHeader("Content-Type", "application/json");
  
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    // Check the response code
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println("Response: " + response);
      //Indicate that we have internet connection and server response by turning on a LED.
      digitalWrite(LED_4_INTERNET, HIGH);      
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
      Serial.println("Error message: " + http.errorToString(httpResponseCode));
    }
    http.end();
  
  } else {
    Serial.println("WiFi Disconnected");
  }

}
*/

void postToAmpXPortal2(int meterNumber = 1) {
  Serial.println("postToAmpXPortal2 function called - Docker API");

  String meterPrefix = "m" + String(meterNumber) + "_";
  
  // Get current timestamp in ISO 8601 format
  String timestamp = getCurrentTimestamp();

  //For debugging and testing, comment out the getCurrentTimestamp function and use the following code.
  //Create timestamp in ISO 8601 format
  /*
  unsigned long seconds = millis() / 1000;
  unsigned long hours = (seconds / 3600) % 24;
  unsigned long minutes = (seconds / 60) % 60;
  unsigned long secs = seconds % 60;
  
  String timestamp = "2025-01-28T";
  if (hours < 10) timestamp += "0";
  timestamp += String(hours) + ":";
  if (minutes < 10) timestamp += "0";
  timestamp += String(minutes) + ":";
  if (secs < 10) timestamp += "0";
  timestamp += String(secs) + "Z";
  */
  
  // Determine serial number for this meter
  String serialNumber;
  if (meterNumber == 1) serialNumber = m1_serial_number;
  else if (meterNumber == 2) serialNumber = m2_serial_number;
  else if (meterNumber == 3) serialNumber = m3_serial_number;
  else if (meterNumber == 4) serialNumber = m4_serial_number;
  else serialNumber = "";

  // Start building the JSON manually for Docker API format
  String httpRequestData = "{";
  httpRequestData += "\"gateway_id\":\"" + String(AMPX_GATEWAY_ID) + "\",";
  httpRequestData += "\"meter_id\":\"" + String(meterNumber) + "\",";
  httpRequestData += "\"serial_number\":\"" + serialNumber + "\",";
  httpRequestData += "\"timestamp\":\"" + timestamp + "\",";
  httpRequestData += "\"values\":{";
  
  // Add all meter readings from JsonDoc to the values object
  bool firstValue = true;
  for (JsonPair kv : JsonDoc.as<JsonObject>()) {
    String key = kv.key().c_str();
    // Only include readings for this meter (starts with correct prefix)
    if (key.startsWith(meterPrefix)) {
      String value = kv.value().as<String>();
      // If this is a name field and value is empty, set default
      if (key.endsWith("_name") && value.length() == 0) {
        value = "New meter";
      }
      if (!firstValue) {
        httpRequestData += ",";
      }
      httpRequestData += "\"" + key + "\":" + (key.endsWith("_name") ? ("\"" + value + "\"") : value);
      firstValue = false;
    }
  }
  
  httpRequestData += "}}"; // Close values object and main object
  
  // Debug: Print the JSON payload and URL
  Serial.println("[DEBUG] JSON payload to be sent:");
  Serial.println(httpRequestData);
  Serial.println("[DEBUG] Target URL:");
  String url = String(ampxportal_server_live);
  //String url = String(ampxportal_server_local);
  Serial.println("URL: " + url);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;   
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
  
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    // Debug: Print HTTP response code and response body
    String response = http.getString();
    Serial.print("[DEBUG] HTTP Response Code: ");
    Serial.println(httpResponseCode);
    Serial.print("[DEBUG] HTTP Response Body: ");
    Serial.println(response);

    // Check the response code
    if (httpResponseCode == 201) {
      Serial.println("✅ SUCCESS: Data sent to Docker API v2!");
      digitalWrite(LED_4_INTERNET, HIGH);
    } else {
      Serial.print("❌ Error on sending POST: ");
      Serial.println(httpResponseCode);
      Serial.println("Error message: " + http.errorToString(httpResponseCode));
    }
    http.end();
  
  } else {
    Serial.println("WiFi Disconnected");
  }
}


void detectNumberOfMeters(){
  debugln("detectNumberOfMeters...");
  debug("MODBUS_TYPE" );
  debugln(MODBUS_TYPE);

  //if (MODBUS_TYPE == MODBUS_TYPE_RS485) {
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
  
  //} else  {
  #else
  
    // MODBUS_TYPE = MODBUS_TYPE_TCPIP
    // TCPIP is only used for handheld meters, there will always just be one.
    numberOfMeters = 1;
  
  //}
  #endif
  
  Serial.println("Number of meters detected: " + String(numberOfMeters)); 
  if (numberOfMeters >= 1){
    //Indicate that one or more meters were detected by turning on a LED.
    digitalWrite(LED_2_METER, HIGH);
  }
  Serial.println("");
}

/* Temporary disabled OTA update to reduce code size.
void doOTAUpdate() {
  Serial.println("Starting OTA update...");

  HTTPClient http;
  http.begin(firmwareURL);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    int length = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    if (!Update.begin(length)) {
      Serial.println("Not enough space for OTA update");
      return;
    }

    // Start updating
    size_t written = Update.writeStream(*stream);
    if (written == length) {
      Serial.println("Firmware written successfully!");
      //TODO customize the update page here
      // send the html with "text/html"
      server.send(200, "text/plain", "Update completed!");
    } else {
      Serial.printf("Written only %d/%d bytes.\n", written, length);
    }

    // Check for success
    if (Update.end()) {
      if (Update.isFinished()) {
        Serial.println("OTA update successful. Restarting...");
        ESP.restart();
      } else {
        Serial.println("OTA update not finished.");
      }
    } else {
      Serial.printf("OTA update failed: %s\n", Update.errorString());
    }
  } else {
    Serial.printf("HTTP request failed. HTTP code: %d\n", httpCode);
  }

  http.end();
}
void handleUpdate() {
  doOTAUpdate();
}

*/