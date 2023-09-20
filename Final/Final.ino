#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>

SoftwareSerial mySerial(7,6); // Pins for A6 module

const int sensorPin = A0;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const float voltageLow = 3.8; // Lower Edge
const float voltageHigh = 4.5; // Higher Edge
RTC_DS1307 rtc; // RTC module object
unsigned long lastTimeMillis = 0;
const unsigned long Interval = 3600000; // One hour interval
unsigned int detected = 0; //avoid repeatedly triggering when rising voltages detected
unsigned int counterstarted = 0; //to denote when counter started
unsigned int msgsent = 0;
unsigned int risingEdgeCount = 0;
unsigned long currentMillis = 0;

void setup() {
  // Set up the LCD's number of columns and rows:
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Prv Hr Count|");
  lcd.setCursor(3, 1);
  lcd.print(0);

  Serial.begin(9600); // Serial monitor
  mySerial.begin(9600); // A6 module
  delay(3000); // Wait for module to initialize

  pinMode(sensorPin, INPUT);
  Wire.begin();
  rtc.begin(); // Initialize RTC module
  
  if (! rtc.begin()){
    Serial.println("Could not find RTC.");
    while(1);
  }
  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__),F(__TIME__))); // Set RTC to compile time if not set
  }
}

void loop() {
  // Read the Sensor voltage
  int sensorValue = analogRead(sensorPin);
  float sensorVoltage = sensorValue * (5.0 / 1023);
  DateTime now = rtc.now(); // Get current time from RTC
  int currentMinute = now.minute();

  Serial.println(currentMinute);  /// Test and delete this
  lcd.setCursor(13, 0);
  lcd.print(":");
  lcd.setCursor(14, 0);
  lcd.print(currentMinute);

  lcd.setCursor(10, 1);
  lcd.print("|");
  lcd.setCursor(11, 1);
  lcd.print(risingEdgeCount);

//condition 01
  if (currentMinute == 59  && counterstarted == 0) {

    delay(2000);
    // Start counting from the next hour
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Prv Hr Count|");
    lcd.setCursor(3, 1);
    lcd.print(risingEdgeCount);
    // Serial.println(risingEdgeCount);
    // Serial.println(currentMinute);
  
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

    msgsent = 1;
    Serial.println(currentMinute);
    risingEdgeCount = 0; // Reset the count
    counterstarted = 1;

    // Serial.println("condition 01");
    // Serial.println(currentMinute);
    // Serial.println(risingEdgeCount);

  }
  //condition 02
  if (currentMinute == 59 && counterstarted == 1){
    if (sensorVoltage >= voltageHigh && detected == 0) {
      risingEdgeCount++;
      detected = 1;
      // Serial.println("condition 02");
      // Serial.println(currentMinute);
      // Serial.println(risingEdgeCount);
    }
    //condition 03
    if (sensorVoltage <= voltageLow && detected == 1) {
      detected = 0;
      // Serial.println("condition 03");
      // Serial.println(currentMinute);
      // Serial.println(risingEdgeCount);
    }
  }

  //condition 04
  else{
    if (sensorVoltage >= voltageHigh && detected == 0) {
      risingEdgeCount++;
      detected = 1;
      // Serial.println("condition 04");
      // Serial.println(currentMinute);
      // Serial.println(risingEdgeCount);
    }

    //condition 05
    if (sensorVoltage <= voltageLow && detected == 1) {
      detected = 0;
      //  Serial.println("condition 05");
      //  Serial.println(currentMinute);
      //  Serial.println(risingEdgeCount);
    }
    counterstarted = 0;
  }
  delay(1);

}

void updateSerial() {
  delay(500);
  while (Serial.available()){
    mySerial.write(Serial.read()); //Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()){
    Serial.write(mySerial.read()); //Forward what Software Serial received to Serial Port
  }
}

