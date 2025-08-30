# AmpX Modbus Library - Project Brief

## Project Overview
The AmpX Modbus Library is an ESP32-based energy monitoring system that communicates with power meters via Modbus protocol (both RS485 and TCP/IP) and provides real-time energy data through web interfaces and remote logging capabilities.

## Core Purpose
- **Energy Monitoring**: Read electrical parameters (voltage, current, power, energy) from power meters
- **Data Communication**: Support both RS485 and TCP/IP Modbus protocols
- **Web Interface**: Provide real-time data visualization through local web server
- **Remote Logging**: Send data to external services (AmpX Portal, EmonCMS)
- **Gateway Functionality**: Act as a bridge between power meters and cloud services

## Key Features
1. **Multi-Protocol Support**: RS485 and TCP/IP Modbus communication
2. **Multi-Meter Support**: Handle up to 4 power meters simultaneously
3. **Real-time Web Interface**: Live data display with WebSocket updates
4. **Remote Data Logging**: Integration with cloud services
5. **Over-the-Air Updates**: Firmware update capability
6. **WiFi Management**: Dynamic WiFi configuration
7. **Status Indicators**: LED status indicators for system health

## Target Hardware
- **Primary**: ESP32 Node32s development board
- **Communication**: RS485 transceiver (MAX485) or Ethernet module
- **Status LEDs**: 4 LEDs for system status indication
- **Power Meters**: Compatible with Modbus-enabled energy meters

## Project Structure
- **Library Core**: `src/AmpXModbus.h` and `src/AmpXModbus.cpp`
- **Examples**: Progressive examples from basic to advanced functionality
- **Versions**: Historical versions showing evolution of the system
- **Documentation**: Hardware connection guides and configuration files
