# Technical Context: AmpX Energy Gateway

## Development Environment
- **IDE**: Arduino IDE / VS Code with PlatformIO
- **Version Control**: Git
- **Documentation**: Markdown

## Hardware Platform
- **Microcontroller**: ESP32 Wroom 32U
- **Network Interface**: 
  - RS485 Version: MAX485 interface
  - TCP/IP Version: W5500 Lite Ethernet
- **Status Indicators**: Four LEDs for power, meter connection, WiFi, and internet/API status (used for real-time feedback and error indication)
- **Power**: 3.3V operating voltage

## Dependencies
- **ESP32 Core**: Arduino-ESP32 framework
- **Modbus Implementation**: Custom implementation for Meatrol meters
- **Web Server**: ESP32 WebServer library
- **WebSockets**: For real-time web updates (by Markus Sattler)
- **ArduinoJson**: Used for API data formatting and meter register definitions
- **WiFiManager**: For WiFi configuration (by tzapu)
- **HTTPClient**: For API communication
- **Preferences**: For persistent storage of settings (planned for API endpoints/credentials)

## Technology Stack
- **Language**: C/C++
- **Protocol**: Modbus (RTU over RS485 or TCP/IP)
- **Web Interface**: HTML, CSS, JavaScript
- **Data Exchange**: JSON for API communication and WebSocket updates
- **Network**: TCP/IP over Ethernet or WiFi
- **Persistent Storage**: NVS (Non-Volatile Storage)

## Technical Implementation Details
- **Memory Usage**: 
  - DynamicJsonDocument (2KB) for meter data
  - String objects for serial numbers and data processing (optimization planned)
  - Multiple global variables
- **Initialization**: Modular functions for NVS, WiFi, NTP, and Ethernet setup
- **Status LEDs**: Used for power, meter, WiFi, and internet/API status

- **Communication Configuration**:
  - RS485: Uses hardware UART with direction control on pins 16/17
  - TCP/IP: Uses W5500 Ethernet with SPI connection

- **Data Polling**:
  - Web updates: 3-second intervals
  - API uploads: 5-minute intervals
  - System restart: Daily (24-hour interval)

- **Multi-Meter Support**:
  - Up to 4 meters currently supported (architecture allows more)
  - Auto-detection of connected meters
  - Individual meter addressing

## Observed Technical Constraints
- **Memory Limitations**: Heavy String usage could lead to fragmentation (planned optimization)
- **Global State**: Many global variables increase complexity
- **Error Recovery**: Improved, but further mechanisms planned
- **Security**: Credentials and API endpoints currently hardcoded (to be moved to persistent storage)
- **API Communication**: HTTP and HTTPS endpoints supported

## Third-Party Integration
- **API Endpoints**:
  - AmpX Portal: https://app.ampx.co/wp-json/ampx-energy/v1/log
  - Docker API: http://192.168.2.32:8080/api/
  - Endpoints and credentials to be moved to persistent storage and web admin

## Build Configuration
- **Conditional Compilation**: `MODBUS_TYPE` defines hardware variant
  - `MODBUS_TYPE_RS485 = 1`
  - `MODBUS_TYPE_TCPIP = 2`
- **Debug Mode**: Configurable via `DEBUG` flag

## Testing State
- Functional testing with live meters required
- No automated testing observed in codebase
- Manual verification of all functionality needed

## Optimization Opportunities
- Move credentials and API endpoints to persistent storage
- Optimize string handling and memory usage
- Further modularize error handling and recovery
- Code cleanup (remove commented sections, consolidate duplication)
- Expand multi-meter support

This document reflects the latest technical context and implementation details from the most recent code review. 