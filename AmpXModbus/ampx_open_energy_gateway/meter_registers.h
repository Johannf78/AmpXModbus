
#ifndef METER_REGISTERS_H
#define METER_REGISTERS_H

#include <ArduinoJson.h>

// Define data type constants
const int dataTypeInt32 = 1;
const int dataTypeInt64 = 2;
const int dataTypeFloat = 3;

// Global JSON document to store meter register definitions
extern JsonDocument MeterRegisterDefs;

// This function sets up the meter register definitions in a JSON document
void setupMeterRegisters() {
  // Format: [register_number, number_of_registers, data_type, friendly_name, json_key]
  
  // Serial Number
  JsonArray serialNumber = MeterRegisterDefs.createNestedArray("serialNumber");
  serialNumber.add(70);           // Register number
  serialNumber.add(2);            // Number of registers to read
  serialNumber.add(dataTypeInt32); // Data type
  serialNumber.add("Serial Number"); // Friendly name
  serialNumber.add("serial");     // JSON key suffix

  // Voltage readings
  JsonArray voltageL1 = MeterRegisterDefs.createNestedArray("voltageL1");
  voltageL1.add(1010);
  voltageL1.add(2);
  voltageL1.add(dataTypeFloat);
  voltageL1.add("Voltage L1");
  voltageL1.add("voltage_L1");
  
  JsonArray voltageL2 = MeterRegisterDefs.createNestedArray("voltageL2");
  voltageL2.add(1012);
  voltageL2.add(2);
  voltageL2.add(dataTypeFloat);
  voltageL2.add("Voltage L2");
  voltageL2.add("voltage_L2");
  
  JsonArray voltageL3 = MeterRegisterDefs.createNestedArray("voltageL3");
  voltageL3.add(1014);
  voltageL3.add(2);
  voltageL3.add(dataTypeFloat);
  voltageL3.add("Voltage L3");
  voltageL3.add("voltage_L3");

  // Current readings
  JsonArray currentL1 = MeterRegisterDefs.createNestedArray("currentL1");
  currentL1.add(1000);
  currentL1.add(2);
  currentL1.add(dataTypeFloat);
  currentL1.add("Current L1");
  currentL1.add("current_L1");
  
  JsonArray currentL2 = MeterRegisterDefs.createNestedArray("currentL2");
  currentL2.add(1002);
  currentL2.add(2);
  currentL2.add(dataTypeFloat);
  currentL2.add("Current L2");
  currentL2.add("current_L2");
  
  JsonArray currentL3 = MeterRegisterDefs.createNestedArray("currentL3");
  currentL3.add(1004);
  currentL3.add(2);
  currentL3.add(dataTypeFloat);
  currentL3.add("Current L3");
  currentL3.add("current_L3");
  
  JsonArray currentAvg = MeterRegisterDefs.createNestedArray("currentAvg");
  currentAvg.add(1006);
  currentAvg.add(2);
  currentAvg.add(dataTypeFloat);
  currentAvg.add("Current Avg");
  currentAvg.add("current_avg");

  // Power readings
  JsonArray activePowerL1 = MeterRegisterDefs.createNestedArray("activePowerL1");
  activePowerL1.add(1028);
  activePowerL1.add(2);
  activePowerL1.add(dataTypeFloat);
  activePowerL1.add("Active Power L1");
  activePowerL1.add("active_power_L1");
  
  JsonArray activePowerL2 = MeterRegisterDefs.createNestedArray("activePowerL2");
  activePowerL2.add(1030);
  activePowerL2.add(2);
  activePowerL2.add(dataTypeFloat);
  activePowerL2.add("Active Power L2");
  activePowerL2.add("active_power_L2");
  
  JsonArray activePowerL3 = MeterRegisterDefs.createNestedArray("activePowerL3");
  activePowerL3.add(1032);
  activePowerL3.add(2);
  activePowerL3.add(dataTypeFloat);
  activePowerL3.add("Active Power L3");
  activePowerL3.add("active_power_L3");
  
  JsonArray activePowerTotal = MeterRegisterDefs.createNestedArray("activePowerTotal");
  activePowerTotal.add(1034);
  activePowerTotal.add(2);
  activePowerTotal.add(dataTypeFloat);
  activePowerTotal.add("Active Power Total");
  activePowerTotal.add("active_power_tot");

  // Energy readings
  JsonArray energyImportedL1 = MeterRegisterDefs.createNestedArray("energyImportedL1");
  energyImportedL1.add(2500);
  energyImportedL1.add(4);
  energyImportedL1.add(dataTypeInt64);
  energyImportedL1.add("Energy Imported L1");
  energyImportedL1.add("active_energy_imported_L1");
  
  JsonArray energyImportedL2 = MeterRegisterDefs.createNestedArray("energyImportedL2");
  energyImportedL2.add(2504);
  energyImportedL2.add(4);
  energyImportedL2.add(dataTypeInt64);
  energyImportedL2.add("Energy Imported L2");
  energyImportedL2.add("active_energy_imported_L2");
  
  JsonArray energyImportedL3 = MeterRegisterDefs.createNestedArray("energyImportedL3");
  energyImportedL3.add(2508);
  energyImportedL3.add(4);
  energyImportedL3.add(dataTypeInt64);
  energyImportedL3.add("Energy Imported L3");
  energyImportedL3.add("active_energy_imported_L3");
  
  JsonArray energyImportedTotal = MeterRegisterDefs.createNestedArray("energyImportedTotal");
  energyImportedTotal.add(2512);
  energyImportedTotal.add(4);
  energyImportedTotal.add(dataTypeInt64);
  energyImportedTotal.add("Energy Imported Total");
  energyImportedTotal.add("active_energy_imported_tot");

  // Print to Serial with pretty formatting
 
  //debugln("MeterRegisterDefs Json.");
  //serializeJsonPretty(MeterRegisterDefs, Serial);
  //debugln(""); // Add a newline at the end

}

#endif // METER_REGISTERS_H 