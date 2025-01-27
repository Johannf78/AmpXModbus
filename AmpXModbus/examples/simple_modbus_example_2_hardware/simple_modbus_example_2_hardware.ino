/*
Read the serial number from the meter.
Added in this example to read the voltage.
Serial number is stored in register 70 on this meter.
ampx_modbus_functions.ino is automatically included as it is in the same folder as this file, all the modbus functions are defined there.
*/


// Define the RS485 control pins
#define MAX485_DE 4 //Green
#define MAX485_RE_NEG 4  //Green
#define RX_PIN 16 //Orange, RO, Receiver Output, RX, Defines the pin for receiving data.
#define TX_PIN 17 //Yellow, DI,  Driver Input, TX, Defines the pin for transmitting data.

void handlePowerMeter()
{
  //This variable are populated from the data read on Modbus, it is reused for different parameters, voltage, current, power, etc.
  uint16_t responseBuffer[4];
  
  // Read registers 70 and 71 - Serial number
  if (readHoldingRegisters(1, 70, 2, responseBuffer)) { // 1 is the Modbus slave ID, adjust if necessary
    Serial.println("");
    Serial.print("Register 70: ");
    Serial.println(responseBuffer[0]);
    Serial.print("Register 71: ");
    Serial.println(responseBuffer[1]);
    uint32_t combinedValue = combineRegistersToInt32(responseBuffer[0], responseBuffer[1]);
    Serial.print("Combined and Swapped Value: ");
    Serial.println(combinedValue);

  } else {
    Serial.println("Error reading registers 70 and 71");
  }

  int meterNumber =1;
  String meterPrefix = "m" + String(meterNumber) + "_";

  // Read voltage on L1
  if (readHoldingRegisters(meterNumber, 1010, 2, responseBuffer)) { // meterNumber is the Modbus slave ID
    processRegisters(responseBuffer, 2, "Voltage L1", meterPrefix + "voltage_L1");
    } else {
    Serial.println("Error reading voltage registers");
  }
}

void processRegisters(uint16_t* results, uint16_t numRegisters,
                      const String& friendlyLabel, const String& docLabel) {
  
  Serial.println("");
  Serial.println(friendlyLabel);
  for (uint16_t i = 0; i < numRegisters; i += 1) {  
    Serial.print("Register " + String(i) + ": ");
    Serial.println(results[i]);
  }
  /*
  uint32_t combinedValue = combineAndSwap(results[1], results[2]);
  Serial.print("combinedValue: ");
  Serial.println(combinedValue);    
  */

  if (numRegisters == 2) {    
    //float value = convertToFloat(results);
    float value = combineRegistersToFloat(results[0], results[1]);
    Serial.print(friendlyLabel + ": ");
    Serial.println(value);    
  } else {
    Serial.print(friendlyLabel + ": ovf");
  }
}

void setup() {
  Serial.begin(9600); // Debug serial
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // Modbus serial
  pinMode(MAX485_DE, OUTPUT);
  pinMode(MAX485_RE_NEG, OUTPUT);
  digitalWrite(MAX485_DE, LOW); //Disables the RS485 driver.
  digitalWrite(MAX485_RE_NEG, LOW); // Enables the RS485 receiver.

  // Debugging information
  Serial.println("Setup complete. Starting communication...");
}

void loop() {
  handlePowerMeter();

  delay(5000); // Wait for a second before the next read
}