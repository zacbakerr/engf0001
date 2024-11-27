#include <Wire.h>
// Define Slave I2C Address. All slaves must be allocated a
// unique number.
#define SLAVE_ADDR 9
// Define the pins used for SDA and SCL. This is important because
// there is a problem with the TTGO and I2C will not work properly
// unless you do.
#define I2C_SDA 21
#define I2C_SCL 22
void setup() {
 Wire.begin (I2C_SDA, I2C_SCL); // Configure the pins
 Serial.begin(9600); // start serial for output
}
void loop() {
Wire.requestFrom(SLAVE_ADDR, 20); // request 6 bytes from slave
 // device SLAVE_ADDR
 String received_string = "";
 while (Wire.available()) { // slave may send less than requested
 char c = Wire.read(); // receive a byte as character
 received_string += c;
}
 Serial.println(received_string); // print the character
 
 delay(500);
}