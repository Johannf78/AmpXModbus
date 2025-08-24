
#if MODBUS_TYPE == MODBUS_TYPE_TCPIP
  void initEthernet(){
    debugln("Starting Ethernet connection...");

    //Set the CS pin, required for ESP32 as the arduino default is different
    Ethernet.init(ETH_SPI_SCS_PIN); 

    
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
      debug("To test LAN connection, please ping: ");
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
    debugln("\nTesting network connectivity between gateway and meter on LAN cable...");
    if (modbus_test_connection()) {
      debugln("Modbus connection test successful!  We are able to communicate with the meter with modbus over TCPIP!");
      //Turn on LED 2 to indicate successful connection to energy meter.
      digitalWrite(LED_2_METER, HIGH);
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
#endif