/*
 * Connections
 * connect SDA of the RTC with A4
 * connect SCL of the RTC with A5
 */
 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "RTClib.h"
 
RTC_DS1307 rtc;
 
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
 
int Day; 
int Month;
int Year; 
int Secs;
int Minutes;
int Hours;
 
String dofweek; // day of week
 
String myDate; 
String myTime; 
 
// for the 16x2 LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
void setup () 
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  //lcd.begin(16, 2);
  delay(3000); // wait for console opening
 
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  // if (rtc.lostPower()) {
  //   Serial.println("RTC lost power, lets set the time!");
  
  // Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
    // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
    // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
}
 
void loop () 
{
DateTime now = rtc.now();
lcd.clear();        
Day = now.day(); 
Month = now.month(); 
Year = now.year();
Secs = now.second(); 
Hours = now.hour(); 
Minutes = now.minute(); 
dofweek = daysOfTheWeek[now.dayOfTheWeek()];  
 
myDate = myDate +dofweek+ " "+ Day + "/" + Month + "/" + Year ; 
myTime = myTime + Hours +":"+ Minutes +":" + Secs ; 
// send to serial monitor
Serial.println(dofweek); 
Serial.println(myDate); 
Serial.println(myTime);
//Print on lcd
lcd.setCursor(0,0);
lcd.print(myDate); 
lcd.setCursor(0,1); 
lcd.print(myTime); 
myDate = "";   
myTime = ""; 
delay(1000);
}