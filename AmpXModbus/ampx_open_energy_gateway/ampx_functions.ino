void initNvs() {
  if (preferences.begin("app", false)) {
    debugln("NVS initialized successfully!");
  } else {
    debugln("Failed to initialize NVS.");
  }
}

void initWiFi() {
  
  WiFi.setHostname("AmpX-Energy-Gateway");

  // Connect to the Wi-Fi network
  /*WiFi.begin(ssid, password);

  debug("Now connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    debug(".");//Print more dots while connecting 
  }*/

  WiFiManager wifiManager;

  // If you've previously connected to your WiFi with this ESP32,
  // WiFi manager will more than likely not do anything.
  // Uncomment this if you want to force it to delete your old WiFi details.
  //wifiManager.resetSettings();

  //Tries to connect to last known WiFi details
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AmpX-Energy-Gateway-AP", "")) {
    debugln("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }
  //Indicate that Wifi is successfully connected by putting on LED 3
  digitalWrite(LED_3_WIFI, HIGH);
  debugln("");
  debugln("Connected to Wi-Fi");
  debug("IP address: ");
  debugln(WiFi.localIP());
  debug("Signal strength: ");
  debugln(WiFi.RSSI());
  debug("WiFi.getHostname: ");
  debugln(WiFi.getHostname());

  // OTA setup
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
    
    debugln("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    debugln("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    #if DEBUG == 1
      Serial.printf("Progress: %u%%\r", (progress * 100) / total);
    #endif
  });

  ArduinoOTA.onError([](ota_error_t error) {
    #if DEBUG == 1
      Serial.printf("Error[%u]: ", error);
    #endif
    if (error == OTA_AUTH_ERROR) {
      debugln("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      debugln("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      debugln("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      debugln("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      debugln("End Failed");
    }
  });

  ArduinoOTA.begin();
  debugln("Ready for OTA updates.");
}

void initServer() {
  server.on("/", handleRoot);
  server.on("/update", HTTP_POST, handleUpdate);
  server.on("/settings", handleSettings);
  server.on("/update_meters_name", HTTP_POST, handleChangeMetersName);
  server.begin();
  //Initialise the websockets on port 81
  webSocket.begin();
}

void initEthernet(){
  debugln("Starting Ethernet connection...");

  //Set the CS pin, required for ESP32 as the arduino default is different
  Ethernet.init(ETH_SPI_SCS); 

  
  debugln("\nStarting Custom Modbus TCP Implementation");
  
  // Initialize ethernet
  debugln("Initializing Ethernet...");
  Ethernet.begin(mac, ip, gateway, subnet);
  
  // Wait for Ethernet to be ready
  delay(2000);

  //Hardware check
  debugln("Checking Ethernet hardware...");
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    debugln("ERROR: No Ethernet hardware detected!");
    return;
  }
  else {
    debugln("Ethernet hardware detected!");
  }
  
  //Check if cable is connected
  if (Ethernet.linkStatus() == LinkOFF) {
    debugln("Link is OFF. Check cable connection.");
  }
  else {
    debugln("Link is ON. Cable is connected. Ready to go!");
    debug("To test connection, please ping: ");
    debugln(ip);
  }

  debug("Arduino IP: ");
  debugln(Ethernet.localIP());
  debug("Energy Meter IP: ");
  debugln(meter_ip);
  debug("Port: ");
  debugln(MODBUS_PORT);

  
  // Initialize Modbus with meter IP
  modbus_init(meter_ip);
  
  // Test network connectivity
  debugln("\nTesting network connectivity...");
  if (modbus_test_connection()) {
    debugln("Connection test successful!");
  } else {
    debugln("Connection test failed!");
    debugln("Please check:");
    debugln("1. Physical network connection");
    debugln("2. IP addresses and subnet mask");
    debugln("3. No firewall blocking port 502");
    debugln("4. Energy meter is powered on and responding");
  }
  
  debugln("Modbus TCPIP Setup complete");
  debugln("");
}

void handleChangeMetersName() {
  String m1_name = server.arg("m1_name");
  preferences.putString("m1_name", m1_name);
  debugln("m1_name: " + m1_name);

  String m2_name = server.arg("m2_name");
  preferences.putString("m2_name", m2_name);
  debugln("m2_name: " + m2_name);

  String m3_name = server.arg("m3_name");
  preferences.putString("m3_name", m3_name);
  debugln("m3_name: " + m3_name);

  String m4_name = server.arg("m4_name");
  preferences.putString("m4_name", m4_name);
  debugln("m4_name: " + m4_name);


  server.send(200, "text/html", "Updated successfully. <br>Back to:<a href='/'>Home</a> | <a href='/settings'>Settings</a>");
}

void handleSettings()
{
  // Load a fresh copy of the settings page from PROGMEM
  webpage_settings.load();
  
  //Use javascript to hide settings for meters not present.
  webpage_settings.replace("numberOfMetersValue", String(numberOfMeters));
  webpage_settings.replace("m_connected_meters_num", String(numberOfMeters));
  webpage_settings.replace("m_gateway_id", String(AMPX_GATEWAY_ID));

  String m1_name = preferences.getString("m1_name");
  debugln("m1_name_value: " + m1_name);
  webpage_settings.replace("m1_name_value", m1_name);

  String m2_name = preferences.getString("m2_name");
  debugln("m2_name_value: " + m2_name);
  webpage_settings.replace("m2_name_value", m2_name);

  //Replace the string m1_serial_number with the actual serial number, done here as it does not update regularly like values.
  webpage_settings.replace("m1_serial_number", m1_serial_number);
  webpage_settings.replace("m2_serial_number", m2_serial_number);
  webpage_settings.replace("m3_serial_number", m3_serial_number);
  webpage_settings.replace("m4_serial_number", m4_serial_number);

  int rssi = WiFi.RSSI();
  // -100 - -30dbm to 0 - 100%
  int percentage = (int)((float)(rssi + 100) * 1.4286);
  if (percentage < 0)
    percentage = 0;
  else if (percentage > 100)
    percentage = 100;
  JsonDoc["m_wifi_rssi"] = String(rssi) + "dBm (" + String(percentage) + "%)";

  server.send(200, "text/html", webpage_settings.getContent());
}

void handleRoot() {
  //String html = "<h1>AmpX Open Energy Gateway</h1>";
  //String vol = "<h1>Voltage on L1: " + String(voltage_on_L1, 2) + "(V)</h1>";
  //html += vol;
  //the webpage has been defined in the included file webpage.h
  
  // Load a fresh copy of the webpage from PROGMEM
  webpage.load();

  // Replace placeholders with actual values
  webpage.replace("m1_serial_number", m1_serial_number);
  webpage.replace("m2_serial_number", m2_serial_number);
  webpage.replace("m3_serial_number", m3_serial_number);
  webpage.replace("numberOfMetersValue", String(numberOfMeters));

  /*
  for (int i = 1; i <= maxNumberOfMeters; i++)
  {
    String meterPrefix = "m" + String(i) + "_";
    //Copy the saved meter name from the preferences to the json doc, this is so that the name can be displayed on the page, even if the name has been changed recently.
    JsonDoc[meterPrefix + "name"] = preferences.getString(meterPrefix + "name");
  }
  */
  JsonDoc["m1_name"] = preferences.getString("m1_name");
  JsonDoc["m2_name"] = preferences.getString("m2_name");
  JsonDoc["m3_name"] = preferences.getString("m3_name");
  JsonDoc["m4_name"] = preferences.getString("m4_name");

  // Send the processed webpage to the client
  server.send(200, "text/html", webpage.getContent());
}

void processRegisters(uint16_t* registerData, uint16_t numRegisters, int registerDataType,
                      const String& friendlyLabel, const String& docLabel) {

  //debugln("");
  //debugln(friendlyLabel);  
  
  for (int i = 0; i < numRegisters; i++) {
    //debug("registerData " + String(i) + ": ");
    //debugln(registerData[i]);
  }
  
  String stringValue = "";
  
  // Process based on the data type
  if (registerDataType == dataTypeInt32) {
    //debugln("dataTypeInt32");
    uint32_t combinedValue = combineRegistersToInt32(registerData[0], registerData[1]);
    stringValue = String(combinedValue);
  } else if (registerDataType == dataTypeInt64) {
    //debugln("dataTypeInt64");
    uint64_t combinedValue = combineRegistersToInt64(registerData[0], registerData[1], registerData[2], registerData[3]);
    stringValue = String((unsigned long)combinedValue); //This is a kludge, as String() does not support uint64_t
  } else if (registerDataType == dataTypeFloat) {
    //debugln("dataTypeFloat");
    float combinedValue = combineRegistersToFloat(registerData[0], registerData[1]);
    stringValue = String(combinedValue, 2); // 2 decimal places
  }
  
  // Write the value to the JsonDoc
  if (stringValue != "") {
    //debug("docLabel: " + docLabel + ", ");
    //debug(friendlyLabel);
    //debug(": ");
    //debugln(stringValue);
    JsonDoc[docLabel] = stringValue;
  }
}

bool handleRs485Register(int meterNumber, int registerNumber, uint16_t numRegisters, const String& friendlyLabel, const String& meterFieldAlias, int registerDataType) {
  uint16_t registerData[4]; // Buffer to hold register values (max 4 for our needs)
  if (modbus_read_registers_rs485(meterNumber, registerNumber, numRegisters, registerData)) {
    String docLabel = "m" + String(meterNumber) + "_" + meterFieldAlias;
    processRegisters(registerData, numRegisters, registerDataType, friendlyLabel, docLabel);
    return true;
  } else {
    debugln("Error reading register " + String(registerNumber));
    return false;
  }
}

bool handleTcpIpRegister(int registerNumber, uint16_t numRegisters, const String& friendlyLabel, const String& meterFieldAlias, int registerDataType, int meterNumber) {
  uint16_t registerData[4]; // Buffer to hold register values (max 4 for our needs)
  if (modbus_read_registers_tcpip(registerNumber, numRegisters, registerData)) {
    String docLabel = "m" + String(meterNumber) + "_" + meterFieldAlias;
    processRegisters(registerData, numRegisters, registerDataType, friendlyLabel, docLabel);
    return true;
  } else {
    debugln("Error reading register " + String(registerNumber));
    return false;
  }
}

void handlePowerMeterRS485(int meterNumber = 1) {
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
    
    // Read the registers
    if (modbus_read_registers_rs485(meterNumber, registerNumber, numRegisters, registerData)) {
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
}

void handlePowerMeterTCPIP(int meterNumber = 1) {
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
    
    // For TCP/IP, we don't use the meter number in the function call since it's set during initialization
    if (modbus_read_registers_tcpip(registerNumber, numRegisters, registerData)) {
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
}

void handleWebSocket() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  
  // Update at most every 1 second
  if (now - lastUpdate >= 1000) {
    String JsonString;
    serializeJson(JsonDoc, JsonString);
    webSocket.broadcastTXT(JsonString);
    //debugln("Sent JSON to websocket");
    //debugln(JsonString);
    lastUpdate = now;
  }
}

void postToEmonCMS(int meterNumber = 1) {
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;

    String valuesString = "";
    valuesString += "&json={";
    
    //This is the list of values to extract from the JsonDoc
    String valuesList[] = {"v_l1", "v_l2", "v_l3", "i_l1", "i_l2", "i_l3", "p_l1", "p_l2", "p_l3"};
    
    //  debugln("valuesString:  " + valuesString);
    
    String valuesString2;
    for (int i = 0; i < sizeof(valuesList) / sizeof(valuesList[0]); i++) {
      String key = "m" + String(meterNumber) + "_" + valuesList[i];
      valuesString2 += valuesList[i] + ":" + JsonDoc[key].as<String>() + ",";
    }
    
    // debugln("valuesString2:  " + valuesString2);
    
    // Remove the trailing comma and add the closing brace
    valuesString2 = valuesString2.substring(0, valuesString2.length() - 1);
    valuesString2 += "}";
    
    debugln("valuesString2:  " + valuesString2);
    
    String url = String(emoncms_server) + "/input/post?node=ampxgw_" + 
                String(meterNumber) + 
                "&apikey=" + String(api_key) + 
                valuesString2;
    
    //debugln(url);
    
    http.begin(url);
    int httpResponseCode = http.GET();
    
    String response = http.getString();
    //debugln(httpResponseCode);
    debug("Https client Response: ");
    debugln(response);
    if (httpResponseCode <= 0) {
      debug("Error on sending POST: ");
      debugln(httpResponseCode);
    }
    
    http.end();
  } else {
    debugln("WiFi Disconnected");
  }
}

void postToAmpXPortal(int meterNumber = 1) {
  debugln("postToAmpXPortal function called");
  
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    
    // Create the data for the POST request
    String httpRequestData = "gateway_id=" + String(AMPX_GATEWAY_ID);
    httpRequestData += "&meternumber=" + String(meterNumber);
    String valuesList[] = {"name", "v_l1", "v_l2", "v_l3", "i_l1", "i_l2", "i_l3", "p_l1", "p_l2", "p_l3", "serial"};
    
    for (int i = 0; i < sizeof(valuesList) / sizeof(valuesList[0]); i++) {
      httpRequestData += "&" + valuesList[i] + "=" + JsonDoc["m" + String(meterNumber) + "_" + valuesList[i]].as<String>();
    }
    
    debugln("httpRequestData:  " + httpRequestData);
    
    // Your Domain name with URL path or IP address with path
    #ifdef LOCAL
      String url = String(ampxportal_server_local) + "/api/device/meter";
    #else 
      String url = String(ampxportal_server_live) + "/api/device/meter";
    #endif
    
    debugln("URL: " + url);
    
    // Specify content-type header
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Send HTTP POST request and get response code
    int httpResponseCode = http.POST(httpRequestData);
    
    // Get response
    String response = http.getString();
    
    // Check result
    debugln(httpResponseCode);
    debugln("Response: " + response);
    
    // If error, print details
    if (httpResponseCode <= 0) {
      debug("Error on sending POST: ");
      debugln(httpResponseCode);
      debugln("Error message: " + http.errorToString(httpResponseCode));
    }
    
    http.end();
  } else {
    debugln("WiFi Disconnected");
  }
}

void detectNumberOfMeters(){
  for (int i = 1; i <= maxNumberOfMeters; i++) {
    uint16_t registerData[2];
    if (MODBUS_TYPE == MODBUS_TYPE_RS485) {
      if (modbus_read_registers_rs485(i, 70, 2, registerData)) {
        uint32_t combinedValue = combineRegistersToInt32(registerData[0], registerData[1]);
        numberOfMeters++;
        //debug("Serial Number: ");
        //debugln(combinedValue);
      }
    } else {
      debugln("Error reading meter: " + String(i));
    }
  }
  //Use this is only one meter is connected via TCP/IP
  if (MODBUS_TYPE == MODBUS_TYPE_TCPIP) {
    numberOfMeters = 1;
  }
  debugln("Number of meters detected: " + String(numberOfMeters));
}

void handleUpdate() {
  // Check if a file was submitted
  if (server.hasArg("update") && server.arg("update") == "true") {
    debugln("Starting OTA update...");
    
    // Get file from server
    HTTPClient http;
    http.begin(firmwareURL);
    
    // Get the file size
    int httpCode = http.GET();
    if (httpCode > 0) {
      WiFiClient *client = http.getStreamPtr();
      int contentLength = http.getSize();
      
      // Check if there's enough space
      if (!Update.begin(contentLength)) {
        debugln("Not enough space for OTA update");
        server.send(200, "text/plain", "Not enough space for update");
        return;
      }
      
      // Write the file
      size_t written = Update.writeStream(*client);
      
      if (written == contentLength) {
        debugln("Firmware written successfully!");
      } else {
        #if DEBUG == 1
          Serial.printf("Written only %d/%d bytes.\n", written, length);
        #endif
      }
      
      if (Update.end()) {
        if (Update.isFinished()) {
          debugln("OTA update successful. Restarting...");
          server.send(200, "text/plain", "Update successful. Restarting...");
          ESP.restart();
        } else {
          debugln("OTA update not finished.");
        }
      } else {
        #if DEBUG == 1
          Serial.printf("OTA update failed: %s\n", Update.errorString());
        #endif
      }
    }
    http.end();
  }
  
  // Show update form
  server.send(200, "text/html", "<html><body><h1>Update Firmware</h1><form method='post' action='/update'><input type='hidden' name='update' value='true'><input type='submit' value='Update Firmware'></form></body></html>");
}
