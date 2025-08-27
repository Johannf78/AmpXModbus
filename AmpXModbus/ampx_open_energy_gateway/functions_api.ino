//API to post data to the AmpX Portal and EmonCMS

//Post data to the AmpX Portal
void postToAmpXPortal2(int meterNumber = 1) {
    debugln("postToAmpXPortal2 function called - Docker API");
  
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
    debugln("[DEBUG] JSON payload to be sent:");
    debugln(httpRequestData);
    debugln("[DEBUG] Target URL:");
    String url = String(ampxportal_server_live);
    //String url = String(ampxportal_server_local);
    debugln("URL: " + url);
  
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;   
      http.begin(url);
      http.addHeader("Content-Type", "application/json");
    
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
  
      // Debug: Print HTTP response code and response body
      String response = http.getString();
      debug("[DEBUG] HTTP Response Code: ");
      debugln(httpResponseCode);
      debug("[DEBUG] HTTP Response Body: ");
      debugln(response);
  
      // Check the response code
      if (httpResponseCode == 201) {
        debugln("✅ SUCCESS: Data sent to Docker API v2!");
        digitalWrite(LED_4_INTERNET, HIGH);
        digitalWrite(LED_5_SERVER, HIGH);
      } else {
        debug("❌ Error on sending POST: ");
        digitalWrite(LED_5_SERVER, LOW);
        debugln(httpResponseCode);
        debugln("Error message: " + http.errorToString(httpResponseCode));

          // Only test internet on connection-related errors
        if (httpResponseCode == HTTPC_ERROR_CONNECTION_REFUSED || 
            httpResponseCode == HTTPC_ERROR_CONNECTION_LOST ||
            httpResponseCode == HTTPC_ERROR_READ_TIMEOUT) {
        
        if (testInternetConnection()) {
          debugln("✅ Internet is working - API issue might be server-side");
          digitalWrite(LED_4_INTERNET, HIGH);
        } else {
          debugln("❌ No internet connection - this explains the API failure");
          digitalWrite(LED_4_INTERNET, LOW);
        }
      }
      }
      http.end();
    
    } else {
      debugln("WiFi Disconnected");
      digitalWrite(LED_4_INTERNET, LOW);
      digitalWrite(LED_5_SERVER, LOW);
    }
  }

//Post data to EmonCMS
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
  //  debugln("valuesString:  " + valuesString);
  */
  
    String valuesString2 ="";
    valuesString2 = "power1:" + String(JsonDoc[meterPrefix + "active_power_L1"]);
    valuesString2 += ",power2:" + String(JsonDoc[meterPrefix + "active_power_L2"]);
    valuesString2 += ",power3:" + String(JsonDoc[meterPrefix + "active_power_L3"]);
    valuesString2 += ",powert:" + String(JsonDoc[meterPrefix + "active_power_tot"]);
   // debugln("valuesString2:  " + valuesString2);
  
    //Add energy imported values
    valuesString2 += ",energy1:" + String(JsonDoc[meterPrefix + "active_energy_imported_L1"]);
    valuesString2 += ",energy2:" + String(JsonDoc[meterPrefix + "active_energy_imported_L2"]);
    valuesString2 += ",energy3:" + String(JsonDoc[meterPrefix + "active_energy_imported_L3"]);
    valuesString2 += ",energyt:" + String(JsonDoc[meterPrefix + "active_energy_imported_tot"]);
    debugln("valuesString2:  " + valuesString2);
  
  
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
      //debugln(url);
  
        
      http.begin(url);
      int httpResponseCode = http.GET();
      if (httpResponseCode > 0) {
        String response = http.getString();
        //debugln(httpResponseCode);
        debug("Https client Response: ");
        debugln(response);
        digitalWrite(LED_4_INTERNET, HIGH);
        digitalWrite(LED_5_SERVER, HIGH);
      } else {
        debug("Error on sending POST: ");
        digitalWrite(LED_4_INTERNET, LOW);
        digitalWrite(LED_5_SERVER, LOW);
        debugln(httpResponseCode);
        //Responce code -1 means no internet access, data ok?
        if (httpResponseCode == HTTPC_ERROR_CONNECTION_REFUSED || 
            httpResponseCode == HTTPC_ERROR_CONNECTION_LOST ||
            httpResponseCode == HTTPC_ERROR_READ_TIMEOUT) {

            if (testInternetConnection()) {
              debugln("✅ Internet is working - API issue might be server-side");
              digitalWrite(LED_4_INTERNET, HIGH);
            } else {
              debugln("❌ No internet connection - this explains the API failure");
              digitalWrite(LED_4_INTERNET, LOW);
            }
        }
      }
      http.end();
    } else {
      debugln("WiFi is disconnected...");
      digitalWrite(LED_4_INTERNET, LOW);
      digitalWrite(LED_5_SERVER, LOW);
    }
  }


// Test internet connectivity by pinging three servers to see if they are reachable.
bool testInternetConnection() {
  const char* testUrls[] = {
    "http://httpbin.org/get",
    "http://www.google.com",
    "http://www.cloudflare.com"
  };
  
  HTTPClient http;
  http.setTimeout(3000); // 3 second timeout
  
  for (const char* url : testUrls) {
    http.begin(url);
    int httpCode = http.GET();
    http.end();
    
    if (httpCode == HTTP_CODE_OK) {
      debugln("✅ Internet connection successful");
      return true;
    }
  }
  
  debugln("❌ Internet connection failed");
  return false;
}