#include <SoftwareSerial.h>

const int sensorPin = A0;

const float voltageLow = 2.0; //Lower Edge
const float voltageHigh = 4.8; //Higher Edge

unsigned long lastTimeMillis = 0;
const unsigned long Interval = 1000;

unsigned int detected = 0;
unsigned int risingEdgeCount = 0;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
}

void loop(){
  //Read the Sensor voltage
  
  int sensorValue = analogRead(sensorPin);
  float sensorVoltage = sensorValue * (5.0/1023);
  //Serial.println(sensorVoltage);

  if(sensorVoltage >= voltageHigh && detected ==0){
    risingEdgeCount++;
    detected = 1;
  }

  if(sensorVoltage <= voltageLow && detected == 1){
    detected = 0;
  }

  unsigned long currentMillis = millis();

  if (currentMillis - lastTimeMillis >= Interval) {
    lastTimeMillis = currentMillis;
    Serial.print("Rising edges detected in the last minute: ");
    Serial.println(risingEdgeCount);
    risingEdgeCount = 0;  // Reset the count

  }
//delay(10);
}