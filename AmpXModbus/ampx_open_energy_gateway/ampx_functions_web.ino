
void initServer() {
  server.on("/", handleRoot);
  // Commented out to reduce space
  // server.on("/update", HTTP_POST, handleUpdate);
  server.on("/settings", handleSettings);
  server.on("/admin", handleAdmin);
  server.on("/update_meters_name", HTTP_POST, handleChangeMetersName);
  server.begin();
  //Initialise the websockets on port 81
  webSocket.begin();
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

void handleAdmin()
{
  //Handle the admin page. Change the gateway ID, Server and API key, and other settings only ment for admin.
  //ToDo: Not sure if this is needed, as it can just be hard coded... Lets see...
  String page = webpage_admin;
  server.send(200, "text/html", page);
}

void handleWebSocket() {
  String JsonString;

  serializeJson(JsonDoc, JsonString);
  //Send the JSON document to the websocket.
  webSocket.broadcastTXT(JsonString);

  Serial.println("Sent JSON to websocket");
  //Serial.println(JsonString);
  // Pretty print with indentation of 2 spaces
  serializeJsonPretty(JsonDoc, Serial);
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
