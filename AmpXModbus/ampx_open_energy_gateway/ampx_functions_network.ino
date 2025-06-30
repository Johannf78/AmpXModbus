void initWiFi() {
  
  WiFi.setHostname("AmpX-Energy-Gateway");

  // Connect to the Wi-Fi network, this is now handeled by the WiFi Mangager library...
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
  Serial.println("");

  /*
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
  */
}


#if MODBUS_TYPE == MODBUS_TYPE_TCPIP
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
      Serial.print("To test LAN connection, please ping: ");
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
    Serial.println("\nTesting network connectivity between gateway and meter on LAN cable...");
    if (modbus_test_connection()) {
      Serial.println("Connection test successful!");
      //Turn on LED 2 to indicate successful connection to energy meter.
      digitalWrite(LED_2_METER, HIGH);
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
#endif

