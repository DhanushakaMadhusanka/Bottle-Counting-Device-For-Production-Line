#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>

SoftwareSerial mySerial(7, 6); // Pins for A6 module

const int sensorPin = A0;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const float voltageLow = 3.8; // Lower Edge
const float voltageHigh = 4.5; // Higher Edge
RTC_DS1307 rtc; // RTC module object

enum State {
  INIT,
  COUNTING,
  SEND_MESSAGE
};

State currentState = INIT;
unsigned int risingEdgeCount = 0;
unsigned long lastTimeMillis = 0;
const unsigned long Interval = 3600000; // One hour interval
unsigned long currentMillis = 0;
unsigned int detected = 0; // Avoid repeatedly triggering when rising voltages detected

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
  mySerial.begin(9600); // A6 module

  pinMode(sensorPin, INPUT);

  delay(2000); // Wait for module to initialize

  Wire.begin();
  rtc.begin(); // Initialize RTC module

  if (!rtc.begin()) {
    Serial.println("Could not find RTC.");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to compile time if not set
}

void loop() {
  currentMillis = millis();
  
  switch (currentState) {
    case INIT:
      // Initialize the system
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Previous Hr Bott Count");
      lcd.setCursor(0, 1);
      lcd.print("Count = ");
      lcd.print(risingEdgeCount);
      Serial.println("Initializing...");
      currentState = COUNTING;
      break;

    case COUNTING:
      // Read the Sensor voltage
      int sensorValue = analogRead(sensorPin);
      float sensorVoltage = sensorValue * (5.0 / 1023);
      DateTime now = rtc.now(); // Get current time from RTC
      int currentMinute = now.minute();

      lcd.setCursor(14, 1);
      lcd.print(currentMinute);
      lcd.setCursor(13, 1);
      lcd.print(":");
      lcd.setCursor(11, 1);
      lcd.print(risingEdgeCount);

      // Condition 01: Start counting at 10 minutes past the hour
      if (currentMinute == 15 && currentState == COUNTING) {
        risingEdgeCount = 0; // Reset the count
        currentState = SEND_MESSAGE;
      }

      // Condition 02: Increment count when voltage is high
      if (sensorVoltage >= voltageHigh && detected == 0) {
        risingEdgeCount++;
        detected = 1;
      }

      // Condition 03: Reset detection when voltage is low
      if (sensorVoltage <= voltageLow && detected == 1) {
        detected = 0;
      }
      break;

    case SEND_MESSAGE:
      // Send the message and reset state
      sendSMS();
      currentState = INIT;
      break;
  }
}

void sendSMS() {
  String recipientNumber = "+94774853304"; // Replace with recipient's number
  String message = "Previous Hour Bottle Count: " + String(risingEdgeCount);

  mySerial.println("AT"); // Once the handshake test is successful, it will return "OK"
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configure TEXT mode
  updateSerial();

  mySerial.print("AT+CMGS=\"");
  mySerial.print(recipientNumber);
  mySerial.println("\"");
  updateSerial();

  mySerial.print(message); // Text content
  updateSerial();
  mySerial.write(26); // Send Ctrl+Z to indicate the end of the message

  Serial.println("Message sent.");
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read()); // Forward what Software Serial received to Serial Port
  }
}
