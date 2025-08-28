//Initialise the server and websocket
void initServer() {
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    debugln("SPIFFS Mount Failed");
    return;
  }
  debugln("SPIFFS mounted successfully");
  
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

//Handle the root webpage
void handleRoot() {
  
  // Read HTML file from SPIFFS
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    debugln("Failed to open index.html from SPIFFS");
    server.send(500, "text/plain", "Failed to read webpage from SPIFFS");
    return;
  }
  
  //Define the webpage variable and read the file from the SPIFFS file system
  String webpage = file.readString();
  file.close();

  //Replace the string m1_serial_number with the actual serial number, done here as it does not update regularly like values.
  webpage.replace("m1_serial_number", m1_serial_number);
  webpage.replace("m2_serial_number", m2_serial_number);
  webpage.replace("m3_serial_number", m3_serial_number);
  webpage.replace("m4_serial_number", m4_serial_number);
  webpage.replace("%NUMBER_OF_METERS%", String(numberOfMeters));

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

//Handle the settings webpage
void handleSettings()
{

  // Read HTML file from SPIFFS
  File file = SPIFFS.open("/settings.html", "r");
  if (!file) {
    debugln("Failed to open settings.html from SPIFFS");
    server.send(500, "text/plain", "Failed to read webpage from SPIFFS");
    return;
  }

  //Define the webpage variable and read the file from the SPIFFS file system
  String webpage = file.readString();
  file.close();

  //String page = webpage_settings;
  //Use javascript to hide settings for meters not present.
  webpage.replace("numberOfMetersValue", String(numberOfMeters));
  webpage.replace("m_connected_meters_num", String(numberOfMeters));
  webpage.replace("m_gateway_id", String(AMPX_GATEWAY_ID));

  String m1_name = preferences.getString("m1_name");
  debugln("m1_name_value: " + m1_name);
  webpage.replace("m1_name_value", m1_name);

  String m2_name = preferences.getString("m2_name");
  debugln("m2_name_value: " + m2_name);
  webpage.replace("m2_name_value", m2_name);

    //Replace the string m1_serial_number with the actual serial number, done here as it does not update regularly like values.
  webpage.replace("m1_serial_number", m1_serial_number);
  webpage.replace("m2_serial_number", m2_serial_number);
  webpage.replace("m3_serial_number", m3_serial_number);
  webpage.replace("m4_serial_number", m4_serial_number);

  int rssi = WiFi.RSSI();
  // -100 - -30dbm to 0 - 100%
  int percentage = (int)((float)(rssi + 100) * 1.4286);
  if (percentage < 0)
    percentage = 0;
  else if (percentage > 100)
    percentage = 100;
  JsonDoc["m_wifi_rssi"] = String(rssi) + "dBm (" + String(percentage) + "%)";

  server.send(200, "text/html", webpage);
}

//Handle the admin webpage
void handleAdmin()
{
  // Read HTML file from SPIFFS
  File file = SPIFFS.open("/admin.html", "r");
  if (!file) {
    debugln("Failed to open admin.html from SPIFFS");
    server.send(500, "text/plain", "Failed to read webpage from SPIFFS");
    return;
  }

  //Define the webpage variable and read the file from the SPIFFS file system
  String webpage = file.readString();
  file.close();

  //Handle the admin page. Change the gateway ID, Server and API key, and other settings only ment for admin.
  //ToDo: Not sure if this is needed, as it can just be hard coded... Lets see...
  //String page = webpage_admin;
  server.send(200, "text/html", webpage); //Send the webpage to the client

}

void handleWebSocket() {
  String JsonString;

  serializeJson(JsonDoc, JsonString);
  //Send the JSON document to the websocket.
  webSocket.broadcastTXT(JsonString);

  //Serial.println("Sent JSON to websocket");
  //Serial.println(JsonString);
  // Pretty print with indentation of 2 spaces
  //serializeJsonPretty(JsonDoc, Serial);
}

