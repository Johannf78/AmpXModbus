

void initWiFi() {
  
  debugln("Inside initWiFi function. Top of function");
  
  WiFi.setHostname("AmpX-Energy-Gateway");

  // Connect to the Wi-Fi network, this is now handeled by the WiFi Mangager library...
  /*WiFi.begin(ssid, password);

  Serial.print("Now connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");//Print more dots while connecting 
  }*/

  WiFiManager wifiManager;

  /*If you've previously connected to your WiFi with this ESP32,
  WiFi manager will more than likely not do anything.
  Uncomment this if you want to force it to delete your old WiFi details.*/
  //wifiManager.resetSettings();

  /*Tries to connect to last known WiFi details
  if it does not connect it starts an access point with the specified name
  here  "AutoConnectAP"and goes into a blocking loop awaiting configuration*/
  if (!wifiManager.autoConnect("AmpX-Energy-Gateway-AP", "")) {
    debugln("Failed to connect and hit timeout...");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
    //TODO: JF: Fix endless loop if no signal...
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

  debugln("Inside initWiFi function. End of function");
  debugln("");

}