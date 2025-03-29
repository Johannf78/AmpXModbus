void initNvs() {
  if (preferences.begin("app", false)) {
    Serial.println("NVS initialized successfully!");
  } else {
    Serial.println("Failed to initialize NVS.");
  }
}

void initWiFi() {
  
  WiFi.setHostname("AmpX-Energy-Gateway");

  // Connect to the Wi-Fi network
  /*WiFi.begin(ssid, password);

  Serial.print("Now connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");//Print more dots while connecting 
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
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }
  //Indicate that Wifi is successfully connected by putting on LED 3
  digitalWrite(LED_3_WIFI, HIGH);
  Serial.println("");
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal strength: ");
  Serial.println(WiFi.RSSI());
  Serial.print("WiFi.getHostname: ");
  Serial.println(WiFi.getHostname());

  // OTA setup
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
    
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress * 100) / total);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();
  Serial.println("Ready for OTA updates.");
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
  Serial.println("Starting Ethernet connection...");

  //Set the CS pin, required for ESP32 as the arduino default is different
  Ethernet.init(ETH_SPI_SCS); 

  
  Serial.println("\nStarting Custom Modbus TCP Implementation");
  
  // Initialize ethernet
  Serial.println("Initializing Ethernet...");
  Ethernet.begin(mac, ip, gateway, subnet);
  
  // Wait for Ethernet to be ready
  delay(2000);

  //Hardware check
  Serial.println("Checking Ethernet hardware...");
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERROR: No Ethernet hardware detected!");
    return;
  }
  else {
    Serial.println("Ethernet hardware detected!");
  }
  
  //Check if cable is connected
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Link is OFF. Check cable connection.");
  }
  else {
    Serial.println("Link is ON. Cable is connected. Ready to go!");
    Serial.print("To test connection, please ping: ");
    Serial.println(ip);
  }

  Serial.print("Arduino IP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Energy Meter IP: ");
  Serial.println(meter_ip);
  Serial.print("Port: ");
  Serial.println(MODBUS_PORT);

  
  // Initialize Modbus with meter IP
  modbus_init(meter_ip);
  
  // Test network connectivity
  Serial.println("\nTesting network connectivity...");
  if (modbus_test_connection()) {
    Serial.println("Connection test successful!");
  } else {
    Serial.println("Connection test failed!");
    Serial.println("Please check:");
    Serial.println("1. Physical network connection");
    Serial.println("2. IP addresses and subnet mask");
    Serial.println("3. No firewall blocking port 502");
    Serial.println("4. Energy meter is powered on and responding");
  }
  
  Serial.println("Modbus TCPIP Setup complete");
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

void handleSettings()
{
  String page = webpage_settings;
  //Use javascript to hide settings for meters not present.
  page.replace("numberOfMetersValue", String(numberOfMeters));
  page.replace("m_connected_meters_num", String(numberOfMeters));
  page.replace("m_gateway_id", String(AMPX_GATEWAY_ID));

  String m1_name = preferences.getString("m1_name");
  Serial.println("m1_name_value: " + m1_name);
  page.replace("m1_name_value", m1_name);

  String m2_name = preferences.getString("m2_name");
  Serial.println("m2_name_value: " + m2_name);
  page.replace("m2_name_value", m2_name);

    //Replace the string m1_serial_number with the actual serial number, done here as it does not update regularly like values.
  page.replace("m1_serial_number", m1_serial_number);
  page.replace("m2_serial_number", m2_serial_number);
  page.replace("m3_serial_number", m3_serial_number);
  page.replace("m4_serial_number", m4_serial_number);

  int rssi = WiFi.RSSI();
  // -100 - -30dbm to 0 - 100%
  int percentage = (int)((float)(rssi + 100) * 1.4286);
  if (percentage < 0)
    percentage = 0;
  else if (percentage > 100)
    percentage = 100;
  JsonDoc["m_wifi_rssi"] = String(rssi) + "dBm (" + String(percentage) + "%)";

  server.send(200, "text/html", page);
}

void handleRoot() {
  //String html = "<h1>AmpX Open Energy Gateway</h1>";
  //String vol = "<h1>Voltage on L1: " + String(voltage_on_L1, 2) + "(V)</h1>";
  //html += vol;
  //the String webpage has been defined in the included file webpage.h

  //Replace the string m1_serial_number with the actual serial number, done here as it does not update regularly like values.
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

  server.send(200, "text/html", webpage);
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
  String JsonString;

  serializeJson(JsonDoc, JsonString);
  //Send the JSON document to the websocket.
  webSocket.broadcastTXT(JsonString);

  //Serial.println("Sent JSON to websocket");
  //Serial.println(JsonString);
}

void postToEmonCMS(int meterNumber = 1) { 
  //old postToRemoteServer
  String meterPrefix = "m" + String(meterNumber) + "_";

/* for testing 
  int test_power1 = random(1000);
  int test_power2 = random(1000);
  int test_power3 = random(1000);
  int test_powert = random(1000);

  String valuesString ="";
  valuesString = "power1:" + String(test_power1);
  valuesString += ",power2:" + String(test_power2);
  valuesString += ",power3:" + String(test_power3);
  valuesString += ",powert:" + String(test_powert);
//  Serial.println("valuesString:  " + valuesString);
*/

  String valuesString2 ="";
  valuesString2 = "power1:" + String(JsonDoc[meterPrefix + "active_power_L1"]);
  valuesString2 += ",power2:" + String(JsonDoc[meterPrefix + "active_power_L2"]);
  valuesString2 += ",power3:" + String(JsonDoc[meterPrefix + "active_power_L3"]);
  valuesString2 += ",powert:" + String(JsonDoc[meterPrefix + "active_power_tot"]);
 // Serial.println("valuesString2:  " + valuesString2);

  //Add energy imported values
  valuesString2 += ",energy1:" + String(JsonDoc[meterPrefix + "active_energy_imported_L1"]);
  valuesString2 += ",energy2:" + String(JsonDoc[meterPrefix + "active_energy_imported_L2"]);
  valuesString2 += ",energy3:" + String(JsonDoc[meterPrefix + "active_energy_imported_L3"]);
  valuesString2 += ",energyt:" + String(JsonDoc[meterPrefix + "active_energy_imported_tot"]);
  Serial.println("valuesString2:  " + valuesString2);


  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "";
    url = String(emoncms_server);
    url += "/input/post?node=" + String(meterNumber) + "&json={";
    //Test with fixed values
    //url += "power1:100,power2:200,power3:300";
    //Test with random values
    //url += valuesString;
    //Test with actual values
    url += valuesString2;
    url += "}&apikey=" + String(api_key);
    //Serial.println(url);

    

    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      //Serial.println(httpResponseCode);
      Serial.print("Https client Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
      //Responce code -1 means no internet access, data ok?
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}


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


void detectNumberOfMeters(){
  if (MODBUS_TYPE == MODBUS_TYPE_RS485) {
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
    } else
  {
    // MODBUS_TYPE = MODBUS_TYPE_TCPIP
    // TCPIP is only used for handheld meters, there will always just be one.
    numberOfMeters = 1;
  }
  Serial.println("Number of meters detected: " + String(numberOfMeters)); 
  if (numberOfMeters >= 1){
    //Indicate that one or more meters were detected by turning on a LED.
    digitalWrite(LED_2_METER, HIGH);
  }
}


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
