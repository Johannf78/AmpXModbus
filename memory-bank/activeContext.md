# Active Context: AmpX Energy Gateway

## Current Focus
The current development focus is on setting up a proper development environment for ESP32 testing. We've pivoted from using LocalWP (which fights external access) to setting up a Linux development server that the ESP32 can easily connect to.

## Recent Changes
- Created Memory Bank structure with core documentation
- Completed codebase review of existing implementation
- Updated project scope based on clarified requirements
- Discovered that the project already has a working implementation
- Optimized JSON document size by switching to DynamicJsonDocument with increased capacity
- Identified but deferred String optimization for future implementation

## Next Steps
1. Set up Linux development server:
   - Complete WSL2 Ubuntu 24.04 installation
   - Choose development stack (LAMP/Node.js/Python)
   - Configure API endpoints that mirror WordPress structure
   - Set up network access for ESP32 connections
2. Update ESP32 code for development:
   - Configure local development API endpoint
   - Test API communication with new server
   - Validate data upload functionality
3. Resume optimization work:
   - Security enhancements for credentials
   - Memory usage optimization
   - Error handling improvements

## Current Implementation Status
- Basic architecture is in place and working
- RS485 and TCP/IP communication implemented
- Web interface with WebSocket updates working
- API upload functionality implemented
- JSON document size optimized
- Multi-meter support implemented

## Active Decisions
- **JSON Document Approach**: Switched from StaticJsonDocument to DynamicJsonDocument for better memory management
- **String Optimization**: Deferred for future to maintain code simplicity at present
- **Optimization Priority**: Focusing on high-impact, low-effort optimizations first

## Open Questions
- What is the expected peak load (number of meters & frequency) in production?
- Are there specific security requirements for production deployment?
- What metrics are most important for monitoring system health?
- Should error handling be more aggressive (auto-restart) or passive (log-only)?

## Timeline Considerations
- Focus on optimizations that don't require major architectural changes
- Test each optimization independently
- Document all optimizations for future maintenance

This document will be updated regularly as development progresses and context evolves. 