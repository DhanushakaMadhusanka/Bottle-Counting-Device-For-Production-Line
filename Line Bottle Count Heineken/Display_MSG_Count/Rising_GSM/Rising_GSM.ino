// Project coordinate by Amesh Udesha, Dhanushka Madhusanka, Anjana Edirisinghe

#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial mySerial(3,2); // pins for A6 module

const int sensorPin = A0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const float voltageLow = 3.8; // Lower Edge
const float voltageHigh = 4.6; // Higher Edge

unsigned long lastTimeMillis = 0;
const unsigned long Interval = 30000;

unsigned int detected = 0;
unsigned int risingEdgeCount = 0;

void setup() {
  // Set up the LCD's number of columns and rows:
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Previous Hr Bott Count");
  lcd.setCursor(0, 1);
  lcd.print("Count = ");
  lcd.print(0);

  Serial.begin(9600); // Serial monitor
  mySerial.begin(9600);     // A6 module

  pinMode(sensorPin, INPUT);
  
  Serial.println("Initializing..."); 
  delay(2000);              // Wait for module to initialize
  
}

void loop() {
  // Read the Sensor voltage
  int sensorValue = analogRead(sensorPin);
  float sensorVoltage = sensorValue * (5.0 / 1023);

  if (sensorVoltage >= voltageHigh && detected == 0) {
    risingEdgeCount++;
    detected = 1;
  }

  if (sensorVoltage <= voltageLow && detected == 1) {
    detected = 0;
  }

  unsigned long currentMillis = millis();

  if (currentMillis - lastTimeMillis >= Interval) {
    lastTimeMillis = currentMillis;

    // Send SMS
    String recipientNumber = "+94774853304"; // Replace with recipient's number
    String message = "Previous Hour Bottle Count: " + String(risingEdgeCount);

    mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
    updateSerial();

    mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();

    mySerial.print("AT+CMGS=\"");
    mySerial.print(recipientNumber);
    mySerial.println("\"");
    updateSerial();

    mySerial.print(message); //text content
    updateSerial();
    mySerial.write(26);

    Serial.println(" <--Message sent.");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Previous Hr Bott Count");
    lcd.setCursor(0, 1);
    lcd.print("Count = ");
    lcd.print(risingEdgeCount);

    risingEdgeCount = 0; // Reset the count
  }
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
