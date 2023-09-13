#include <SoftwareSerial.h>

SoftwareSerial A6MODULE(2,3); // pins for A6 module

void setup() {
  Serial.begin(9600);       // Serial monitor
  A6MODULE.begin(9600);     // A6 module
  
  delay(1000);              // Wait for module to initialize
  Serial.println("Module initialized.");
  
  // Set module to SMS mode
  A6MODULE.println("AT+CMGF=1");
  delay(1000);
  Serial.println("Set SMS mode.");
}

void loop() {
  // Replace the following variables with the recipient's phone number and the desired message
  String recipientNumber = "+94716123511";
  String message = "Hello from A6 GSM module!";
  
  // Send SMS
  A6MODULE.print("AT+CMGS=\"");
  A6MODULE.print(recipientNumber);
  A6MODULE.println("\"");
  delay(1000);
  A6MODULE.print(message);
  delay(500);
  A6MODULE.write(26); // Ctrl+Z character to send the message
  delay(1000);
  
  Serial.println("Message sent.");
  
  // Wait before sending another message
  delay(1000000);
}
