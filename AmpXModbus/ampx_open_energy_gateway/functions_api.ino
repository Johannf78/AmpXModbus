//API to post data to the AmpX Portal and EmonCMS

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
        debug("Https client Response: ");
        debugln(response);
      } else {
        debug("Error on sending POST: ");
        debugln(httpResponseCode);
        //Responce code -1 means no internet access, data ok?
      }
      http.end();
    } else {
      debugln("WiFi is disconnected...");
    }
  }
