# Technical Context - AmpX Modbus Library

## Hardware Requirements

### Primary Platform
- **ESP32 Node32s**: Main development board
- **Flash Memory**: 4MB (minimum)
- **RAM**: 520KB (sufficient for web server and JSON processing)
- **WiFi**: Built-in 802.11 b/g/n support

### Communication Hardware
#### RS485 Configuration
- **MAX485 Transceiver**: RS485 to TTL conversion
- **Control Pins**: DE (Data Enable) and RE (Receive Enable)
- **Serial Pins**: RX (GPIO 16), TX (GPIO 17)
- **Termination**: 120Ω resistors for proper bus termination

#### TCP/IP Configuration
- **Ethernet Module**: W5500 or similar SPI-based module
- **SPI Pins**: CS (GPIO 5), MOSI, MISO, SCK
- **Network**: Static IP configuration (192.168.2.121)

### Status Indicators
- **LED 1 (Power)**: GPIO 12 - System power indicator
- **LED 2 (Meter)**: GPIO 14 - Meter communication status
- **LED 3 (WiFi)**: GPIO 27 - WiFi connection status
- **LED 4 (Internet)**: GPIO 26 - Internet connectivity status

## Software Dependencies

### Core Libraries
```cpp
#include <Arduino.h>           // ESP32 core
#include <WiFi.h>              // WiFi connectivity
#include <WebServer.h>         // HTTP server
#include <WebSocketsServer.h>  // Real-time communication
#include <ArduinoJson.h>       // JSON processing
#include <WiFiManager.h>       // WiFi configuration
#include <ArduinoOTA.h>        // Over-the-air updates
#include <Preferences.h>       // NVS storage
#include <HTTPClient.h>        // HTTP client
```

### Custom Libraries
- **ampx_modbus_rs485.h**: RS485 Modbus implementation
- **ampx_modbus_tcpip.h**: TCP/IP Modbus implementation

### External Services
- **AmpX Portal**: `https://app.ampx.co/`
- **EmonCMS**: `http://emoncms.org`
- **Firmware Updates**: `https://ampx.co/downloads/`

## Communication Protocols

### Modbus Protocol
- **Function Code 0x03**: Read Holding Registers
- **Data Format**: Big-endian with byte swapping
- **CRC-16**: Standard Modbus CRC calculation
- **Timeout**: 1000ms response timeout
- **Retry Logic**: Built into communication functions

### Network Configuration
```cpp
// Static IP Configuration
IPAddress ip(192, 168, 2, 121);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress meter_ip(192, 168, 2, 122);
```

### Web Server Configuration
- **HTTP Port**: 80
- **WebSocket Port**: 81
- **Content Type**: text/html, application/json
- **CORS**: Not explicitly configured (may need for cross-origin requests)

## Data Processing

### Register Mapping
- **Serial Number**: Registers 70-71 (32-bit integer)
- **Voltage L1/L2/L3**: Registers 1010-1015 (32-bit float)
- **Current L1/L2/L3**: Registers 1000-1007 (32-bit float)
- **Active Power**: Registers 1028-1035 (32-bit float)
- **Energy Imported**: Registers 2500-2515 (64-bit integer)

### Data Conversion
```cpp
// Float conversion with byte swapping
float convertToFloat(uint32_t value) {
  // Byte order: [3][2][1][0] -> [0][1][2][3]
}

// 64-bit integer combination
uint64_t combineAndSwap64(uint16_t reg0, uint16_t reg1, 
                         uint16_t reg2, uint16_t reg3) {
  // Little-endian to big-endian conversion
}
```

## Performance Characteristics

### Timing
- **Meter Reading**: Every 3 seconds
- **Cloud Upload**: Every 5 minutes (300 seconds)
- **System Restart**: Every 24 hours (86400 seconds)
- **WebSocket Updates**: Real-time (immediate)

### Memory Usage
- **JSON Buffer**: 512 bytes (StaticJsonDocument)
- **Meter Data**: ~200 bytes per meter
- **Web Page**: ~2KB HTML
- **Total RAM Usage**: ~50KB (well within ESP32 limits)

### Network Bandwidth
- **Local Web Interface**: ~1KB per request
- **Cloud Upload**: ~500 bytes per meter per upload
- **WebSocket Updates**: ~200 bytes per update

## Development Environment

### Arduino IDE Configuration
- **Board**: ESP32 Dev Module
- **Upload Speed**: 921600 baud
- **CPU Frequency**: 240MHz
- **Flash Size**: 4MB
- **Partition Scheme**: Default 4MB with spiffs

### Debug Configuration
```cpp
#define DEBUG 1
#if DEBUG == 1
  #define debug(x) Serial.print(x)
  #define debugln(x) Serial.println(x)
#else
  #define debug(x)
  #define debugln(x)
#endif
```

### Build Configuration
- **Compiler**: ESP32 Arduino Core
- **Libraries**: Managed through Arduino Library Manager
- **OTA Updates**: Enabled for remote firmware updates
- **NVS Storage**: Used for persistent configuration
