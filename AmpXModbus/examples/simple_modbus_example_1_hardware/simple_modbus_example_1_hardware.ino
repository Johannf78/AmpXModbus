/*
Read the serial number from the meter.
Serial number is stored in register 70 on this meter.
ampx_modbus_functions.ino is automatically included as it is in the same folder as this file, all the modbus functions are defined there.
*/

//This library is required to use digital pins as a software serial port. EspSoftwareSerial - Download and include this libarary in the library manager.
//#include <SoftwareSerial.h>
//If using hardware serial, such as the esp32 or Arduino Mega, then it is not nessessary to use software serial.

// Define the RS485 control pins
#define MAX485_DE 4 //Green
#define MAX485_RE_NEG 4  //Green
#define RX_PIN 16 //Orange, RO, Receiver Output, RX, Defines the pin for receiving data.
#define TX_PIN 17 //Yellow, DI,  Driver Input, TX, Defines the pin for transmitting data.

//SoftwareSerial modbusSerial(RX_PIN, TX_PIN);


void setup() {
  Serial.begin(9600); // Debug serial
  //modbusSerial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // Modbus serial, 
  //SERIAL_8N1: configuration for serial communication. 8: 8 data bits, N: No parity bit, 1: 1 stop bit
  pinMode(MAX485_DE, OUTPUT);
  pinMode(MAX485_RE_NEG, OUTPUT);
  digitalWrite(MAX485_DE, LOW); //Disables the RS485 driver.
  digitalWrite(MAX485_RE_NEG, LOW); // Enables the RS485 receiver.

  // Debugging information
  Serial.println("Setup complete. Starting communication...");
}

void loop() {
  uint16_t results[8];

  // Read registers 70 and 71 - Serial number
  if (readHoldingRegisters(1, 0x46, 2, results)) { // 1 is the Modbus slave ID, adjust if necessary
    Serial.print("Register 70: ");
    Serial.println(results[0]);
    Serial.print("Register 71: ");
    Serial.println(results[1]);
    uint32_t combinedValue = combineAndSwap(results[0], results[1]);
    Serial.print("Combined and Swapped Value: ");
    Serial.println(combinedValue);

  } else {
    Serial.println("Error reading registers 70 and 71");
  }

  delay(5000); // Wait for a second before the next read
}