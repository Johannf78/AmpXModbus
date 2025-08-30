# System Patterns - AmpX Modbus Library

## Architecture Overview
The system follows a modular, event-driven architecture with clear separation of concerns:

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Power Meters  │◄──►│  ESP32 Gateway   │◄──►│  Cloud Services │
│  (Modbus RS485/ │    │                  │    │  (AmpX Portal,  │
│   TCP/IP)       │    │                  │    │   EmonCMS)      │
└─────────────────┘    └──────────────────┘    └─────────────────┘
                              │
                              ▼
                       ┌──────────────────┐
                       │  Web Interface   │
                       │  (Local/Remote)  │
                       └──────────────────┘
```

## Core Design Patterns

### 1. **Modular Communication Layer**
- **Protocol Abstraction**: Common interface for RS485 and TCP/IP
- **Function Separation**: `ampx_modbus_functions.ino` contains all Modbus operations
- **Error Handling**: Consistent error handling across protocols

### 2. **Data Processing Pipeline**
```cpp
Raw Modbus Data → Register Processing → Data Conversion → Web Display/Cloud Upload
```

### 3. **State Management**
- **Global Variables**: Meter data stored in global arrays
- **Static Counters**: Timing control for different operations
- **Preferences**: Persistent storage for configuration

### 4. **Event-Driven Loop**
- **Timer-Based Operations**: 3-second meter reads, 5-minute cloud uploads
- **Continuous Services**: Web server and WebSocket handling
- **Interrupt-Driven**: Serial communication and network events

## Key Components

### 1. **Modbus Communication**
- **CRC Calculation**: Standard Modbus CRC-16 implementation
- **Request/Response**: Synchronous communication with timeout
- **Data Conversion**: Byte swapping for different data types (float, int64)

### 2. **Web Interface**
- **Static HTML**: Embedded HTML with placeholder replacement
- **WebSocket**: Real-time data updates
- **REST API**: JSON endpoints for data access

### 3. **Cloud Integration**
- **HTTP Client**: POST requests to remote servers
- **JSON Serialization**: ArduinoJson for data formatting
- **Authentication**: API key-based authentication

### 4. **System Management**
- **WiFi Management**: WiFiManager for configuration
- **OTA Updates**: Over-the-air firmware updates
- **Status Monitoring**: LED indicators for system health

## Data Flow Patterns

### 1. **Meter Reading Cycle**
```
Timer Trigger → Read Registers → Process Data → Update WebSocket → Store Globally
```

### 2. **Web Request Handling**
```
HTTP Request → Parse Parameters → Generate Response → Send HTML/JSON
```

### 3. **Cloud Upload Process**
```
Timer Trigger → Collect Data → Serialize JSON → HTTP POST → Handle Response
```

## Error Handling Patterns

### 1. **Communication Errors**
- **Timeout Handling**: 1-second timeout for Modbus responses
- **CRC Validation**: Automatic error detection
- **Retry Logic**: Built into communication functions

### 2. **Network Errors**
- **Connection Monitoring**: WiFi status checking
- **Graceful Degradation**: Continue local operation if cloud fails
- **Automatic Recovery**: WiFi reconnection attempts

### 3. **System Errors**
- **Watchdog**: 24-hour automatic restart
- **LED Indicators**: Visual error reporting
- **Serial Debugging**: Comprehensive debug output
