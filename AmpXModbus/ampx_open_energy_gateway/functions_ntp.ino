
void initNTP() {
    debugln("Initializing NTP time synchronization...");
    
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
    debugln("Waiting for NTP time sync: ");
    time_t now = time(nullptr);
    int timeout = 20; // 20 second timeout
    
    while (now < 24 * 3600 && timeout > 0) {  // Wait until we have a valid timestamp
      delay(1000);
      debug(".");
      now = time(nullptr);
      timeout--;
    }
    
    if (timeout > 0) {
      debugln(" SUCCESS!");
      debug("Current time: ");
      debugln(ctime(&now));
    } else {
      debugln(" TIMEOUT!");
      debugln("Failed to sync with NTP servers. Using system time.");
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