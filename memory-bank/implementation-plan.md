# Implementation Plan: AmpX Energy Gateway

## Project Overview
The AmpX Energy Gateway will be a communication solution for Meatrol Brand energy meters, supporting both RS485 and TCP/IP communication variants. It will collect data from up to 32 meters, display it on a local web interface, and upload it to a central API every 15 minutes.

## Implementation Phases

### Phase 1: Communication Layer (2 weeks)

#### RS485 Communication Module
1. Review existing RS485 implementation in the codebase
2. Enhance the implementation for reliability and error handling
3. Create a clear API for higher-level components
4. Add support for multiple meter addresses
5. Implement robust error handling and recovery

#### TCP/IP Communication Module
1. Develop TCP/IP communication module for Meatrol meters
2. Implement Modbus TCP protocol handling
3. Create connection management for TCP/IP meters
4. Ensure robust error handling and reconnection logic
5. Test with Meatrol meter TCP/IP interface

#### Common Interface
1. Create a common interface/abstraction for both communication types
2. Implement appropriate compile-time configuration
3. Ensure consistent behavior regardless of communication type
4. Document the interface for higher-level components

### Phase 2: Meter Interface (2 weeks)

#### Meter Data Structures
1. Define comprehensive data structures for Meatrol meter data
2. Create meter configuration and status tracking
3. Implement register mapping for all relevant meter parameters
4. Create utility functions for data conversion and processing

#### Multi-Meter Support
1. Implement meter discovery and enumeration
2. Create a management system for up to 32 meters
3. Develop unique identification for each meter
4. Implement efficient polling strategies for multiple meters

#### Data Processing
1. Create data validation and error checking
2. Implement unit conversion where necessary
3. Design efficient data structures for web display and API upload
4. Add timestamp and metadata to measurements

### Phase 3: Core Functionality (2 weeks)

#### Data Collection
1. Implement scheduling for regular meter polling
2. Create efficient data storage structures
3. Develop prioritization for multiple meters
4. Implement data aggregation where appropriate

#### Web Interface
1. Enhance existing web interface for tabular data display
2. Add real-time updates via WebSockets
3. Implement responsive design for various devices
4. Add basic configuration options if needed

#### API Integration
1. Develop API communication module
2. Implement JSON formatting for meter data
3. Create scheduling for 15-minute uploads
4. Add error handling and logging for API communication
5. Implement authentication if required

### Phase 4: Testing and Optimization (2 weeks)

#### Unit Testing
1. Develop comprehensive unit tests for each module
2. Create test fixtures and mocks for hardware dependencies
3. Implement automated testing where possible

#### Integration Testing
1. Test with actual Meatrol meters (both RS485 and TCP/IP variants)
2. Verify multi-meter functionality
3. Validate web interface operation
4. Confirm API upload functionality

#### Performance Optimization
1. Review memory usage and optimize as needed
2. Improve response time for web interface
3. Optimize polling schedules for multiple meters
4. Enhance error recovery mechanisms

#### Documentation
1. Create comprehensive system documentation
2. Document API endpoints and data formats
3. Create installation and configuration guide
4. Document troubleshooting procedures

## Implementation Approach

### Code Organization
- Create clear separation between communication layers and business logic
- Use modular design with well-defined interfaces
- Implement conditional compilation for hardware variants
- Maintain backward compatibility where possible

### Testing Strategy
- Test each module independently with unit tests
- Use integration tests for complete system validation
- Test with actual Meatrol meters for both variants
- Verify multi-meter operation with different configurations

### Development Environment
- Use PlatformIO for consistent build environment
- Implement automated testing where possible
- Use version control (Git) for all code changes
- Document all design decisions and trade-offs

## Resource Requirements
- ESP32 Wroom 32U development boards
- MAX485 interface modules for RS485 testing
- W5500 Lite Ethernet modules for TCP/IP testing
- Meatrol Brand energy meters (both RS485 and TCP/IP variants)
- Development workstation with Arduino IDE / PlatformIO

## Risk Mitigation
- Start with existing code as foundation where possible
- Test communication modules extensively before integration
- Create fallback mechanisms for error conditions
- Document all assumptions and limitations

## Timeline Overview
- Phase 1 (Communication Layer): 2 weeks
- Phase 2 (Meter Interface): 2 weeks
- Phase 3 (Core Functionality): 2 weeks
- Phase 4 (Testing and Optimization): 2 weeks
- Total estimated time: 8 weeks

This implementation plan will be adjusted as development progresses and as we gather more information about specific requirements and challenges. 