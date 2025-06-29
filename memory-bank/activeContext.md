# Active Context: AmpX Energy Gateway

## Current Focus
The current development focus is on optimizing and maintaining a robust, unified codebase for the ESP32-based AmpX Energy Gateway. The system now features a single handler for both RS485 and TCP/IP Modbus communication, robust initialization routines, and improved API integration.

## Recent Changes
- Implemented shared Modbus functions for register conversion (float, int32, int64)
- Unified meter handling with `handlePowerMeter` for both RS485 and TCP/IP
- Modular initialization for NVS, WiFi (WiFiManager), and NTP
- Enhanced error handling and status indication via serial and LEDs
- Integrated two API upload methods (WordPress and Docker API)
- Added TODOs for moving credentials/settings to persistent storage and web admin

## Next Steps
1. Move API endpoints and credentials to persistent storage and web admin interface
2. Optimize string handling and reduce memory fragmentation
3. Clean up legacy/commented code and consolidate duplication
4. Continue improving error handling and recovery mechanisms
5. Expand multi-meter support and test with more than 4 meters

## Current Implementation Status
- Unified Modbus handler in place and working
- Robust initialization and connectivity routines
- Web interface and WebSocket updates functional
- Dual API integration (WordPress and Docker API)
- Status LEDs provide real-time feedback

## Active Decisions
- **Unified Handler**: All meter types handled by a single function for maintainability
- **DynamicJsonDocument**: Used for flexible memory management
- **Status LEDs**: Used for real-time feedback on system state

## Open Questions
- What is the expected production scale (meters, frequency)?
- Are there additional security requirements for API endpoints?
- Should all settings be user-configurable via web admin?

## Timeline Considerations
- Prioritize optimizations that improve reliability and maintainability
- Test all changes with live hardware
- Document all new patterns and decisions

This document will be updated regularly as development progresses and context evolves. 