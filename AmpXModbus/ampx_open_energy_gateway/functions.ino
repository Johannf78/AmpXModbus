void initNvs() {
  if (preferences.begin("app", false)) {
    Serial.println("NVS initialized successfully!");
  } else {
    Serial.println("Failed to initialize NVS.");
  }
  Serial.println("");
}

//Add new function prototypes here...
//For all functions that there is not specific category for, add them here.


