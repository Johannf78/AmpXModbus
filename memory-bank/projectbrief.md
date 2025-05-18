# Project Brief: AmpX Energy Gateway

## Overview
The AmpX Energy Gateway project aims to implement a communication gateway for energy meters using ESP32 hardware. This system allows for monitoring and collection of energy measurement data through either Modbus TCP/IP or RS485 interfaces, displaying the data locally, and uploading it to a remote API.

## Core Requirements
- Implement communication with Meatrol Brand energy meters via:
  - Modbus TCP/IP 
  - Modbus RS485 (separate compiled version)
- Read meter serial numbers and measurement data
- Display collected data on a local web interface
- Upload data to a remote API at 15-minute intervals
- Support for up to 32 meters simultaneously

## Project Goals
- Create a stable and responsive energy monitoring gateway
- Enable seamless integration with Meatrol energy meters
- Provide a simple web interface for real-time data viewing
- Ensure reliable data collection and API transmission

## Hardware Specifications
- ESP32 Wroom 32U microcontroller
- MAX485 interface for RS485 communication
- W5500 Lite for Ethernet communication
- Two separate hardware configurations (RS485/TCP)

## Constraints
- ESP32 hardware limitations
- No user configuration of communication protocols (hardcoded)
- No local data storage requirements
- No specific handling for connectivity issues

## Stakeholders
- Energy monitoring system operators
- Integration partners

This document serves as the foundation for all other Memory Bank documents and defines the core scope of the AmpX Energy Gateway project. 