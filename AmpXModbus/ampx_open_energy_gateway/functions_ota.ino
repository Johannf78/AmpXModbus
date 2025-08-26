void initOTA(){

  /*
  //JF Disabled for now to try and save space.
  
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


/* Temporary disabled OTA update to reduce code size.
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

*/