#include "FS.h"
#include "SPIFFS.h"

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 SPIFFS Example ===");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  Serial.println("SPIFFS mounted successfully");
  
  // List all files in SPIFFS
  listSPIFFSFiles();
  
  // Read a file
  readFileExample();
  
  // Write a file
  writeFileExample();
  
  // Check SPIFFS info
  showSPIFFSInfo();
}

void loop() {
  // Nothing to do in loop for this example
  delay(1000);
}

void listSPIFFSFiles() {
  Serial.println("\n=== Files in SPIFFS ===");
  
  File root = SPIFFS.open("/");
  if (!root) {
    Serial.println("Failed to open SPIFFS root directory");
    return;
  }
  
  File file = root.openNextFile();
  while (file) {
    Serial.printf("File: %-20s Size: %6d bytes\n", file.name(), file.size());
    file = root.openNextFile();
  }
  root.close();
  
  if (!file) {
    Serial.println("No files found in SPIFFS");
  }
}

void readFileExample() {
  Serial.println("\n=== Reading File Example ===");
  
  // Try to read index.html
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    Serial.println("Failed to open /index.html");
    return;
  }
  
  Serial.printf("File size: %d bytes\n", file.size());
  Serial.println("First 200 characters:");
  
  String content = file.readString();
  file.close();
  
  // Show first 200 characters
  int maxChars = min(200, (int)content.length());
  Serial.println(content.substring(0, maxChars));
  if (content.length() > 200) {
    Serial.println("...(truncated)");
  }
}

void writeFileExample() {
  Serial.println("\n=== Writing File Example ===");
  
  // Write a test file
  File file = SPIFFS.open("/test.txt", "w");
  if (!file) {
    Serial.println("Failed to create /test.txt");
    return;
  }
  
  file.println("Hello from ESP32!");
  file.println("Current millis: " + String(millis()));
  file.close();
  
  Serial.println("Successfully wrote /test.txt");
  
  // Read it back
  file = SPIFFS.open("/test.txt", "r");
  if (file) {
    Serial.println("Content of /test.txt:");
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
  }
}

void showSPIFFSInfo() {
  Serial.println("\n=== SPIFFS Information ===");
  
  size_t totalBytes = SPIFFS.totalBytes();
  size_t usedBytes = SPIFFS.usedBytes();
  size_t freeBytes = totalBytes - usedBytes;
  
  Serial.printf("Total space: %d bytes (%.1f KB)\n", totalBytes, totalBytes/1024.0);
  Serial.printf("Used space:  %d bytes (%.1f KB)\n", usedBytes, usedBytes/1024.0);
  Serial.printf("Free space:  %d bytes (%.1f KB)\n", freeBytes, freeBytes/1024.0);
  Serial.printf("Usage: %.1f%%\n", (usedBytes * 100.0) / totalBytes);
}
