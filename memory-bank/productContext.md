# Product Context - AmpX Modbus Library

## Problem Statement
Energy monitoring systems often require:
- **Protocol Flexibility**: Support for different communication methods (RS485, TCP/IP)
- **Real-time Monitoring**: Live data visualization and alerts
- **Remote Access**: Cloud connectivity for data logging and analysis
- **Scalability**: Support for multiple meters in industrial/commercial settings
- **Reliability**: Robust communication with automatic error handling

## Solution Approach
The AmpX Modbus Library provides a comprehensive energy monitoring solution that:

### 1. **Universal Communication**
- Supports both RS485 and TCP/IP Modbus protocols
- Automatic meter detection and configuration
- Robust error handling and retry mechanisms

### 2. **Real-time Data Access**
- Local web interface for immediate monitoring
- WebSocket-based live updates
- JSON API for data integration

### 3. **Cloud Integration**
- Direct integration with AmpX Portal
- EmonCMS compatibility for open-source energy monitoring
- Configurable remote logging intervals

### 4. **User Experience**
- WiFi auto-configuration through WiFiManager
- Over-the-air firmware updates
- Visual status indicators (LEDs)
- Mobile-responsive web interface

## Target Users
- **Industrial Facilities**: Monitor multiple electrical circuits
- **Commercial Buildings**: Energy consumption tracking
- **Renewable Energy Systems**: Solar/wind power monitoring
- **Research & Development**: Energy efficiency studies
- **Smart Home Enthusiasts**: Advanced energy monitoring

## Value Proposition
- **Cost-Effective**: ESP32-based solution reduces hardware costs
- **Flexible**: Supports various meter types and communication methods
- **Scalable**: Handle 1-4 meters with single gateway
- **Reliable**: Built-in error handling and automatic recovery
- **Future-Proof**: OTA updates and extensible architecture
