const byte PulsesPerRevolution = 1;
const unsigned long ZeroTimeout = 100000;
const byte numReadings = 2;

volatile unsigned long LastTimeWeMeasured;
volatile unsigned long PeriodBetweenPulses = ZeroTimeout + 1000;
volatile unsigned long PeriodAverage = ZeroTimeout + 1000;
unsigned long FrequencyRaw;
unsigned long FrequencyReal;
unsigned long RPM;
unsigned int PulseCounter = 1;
unsigned long PeriodSum;

unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;
unsigned long CurrentMicros = micros();
unsigned int AmountOfReadings = 1;
unsigned int ZeroDebouncingExtra;
unsigned long readings[numReadings];
unsigned long readIndex;
unsigned long total;
unsigned long average;

int sensorValue;

void setup() {
  Serial.begin(9600);

  attachInterrupt(sensorValue, Pulse_Event, RISING);  // Use(interrupt 0) on Arduino Uno
  delay(1);
}

void loop() {
  sensorValue = analogRead(A0);

  LastTimeCycleMeasure = LastTimeWeMeasured;
  CurrentMicros = micros();
  if (CurrentMicros < LastTimeCycleMeasure) {
    LastTimeCycleMeasure = CurrentMicros;
  }
  FrequencyRaw = 10000000000UL / PeriodAverage;  // UL is added for unsigned long literal
  if (PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra) {
    FrequencyRaw = 0;  // Set frequency as 0.
    ZeroDebouncingExtra = 2000;
  } else {
    ZeroDebouncingExtra = 0;
  }
  FrequencyReal = FrequencyRaw / 100000;

  RPM = FrequencyRaw / PulsesPerRevolution * 60;
  RPM = RPM / 10000;
  total = total - readings[readIndex];
  readings[readIndex] = RPM;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  average = total / numReadings;

  Serial.print("Period: ");
  Serial.print(PeriodBetweenPulses);
  Serial.print("\tReadings: ");
  Serial.print(AmountOfReadings);
  Serial.print("\tFrequency: ");
  Serial.print(FrequencyReal);
  Serial.print("\tRPM: ");
  Serial.print(RPM);
  Serial.print("\tTachometer: ");
  Serial.println(average);
}

void Pulse_Event() {
  PeriodBetweenPulses = micros() - LastTimeWeMeasured;
  LastTimeWeMeasured = micros();
  if (PulseCounter >= AmountOfReadings) {
    PeriodAverage = PeriodSum / AmountOfReadings;
    PulseCounter = 1;
    PeriodSum = PeriodBetweenPulses;

    int RemappedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);
    RemappedAmountOfReadings = constrain(RemappedAmountOfReadings, 1, 10);
    AmountOfReadings = RemappedAmountOfReadings;
  } else {
    PulseCounter++;
    PeriodSum = PeriodSum + PeriodBetweenPulses;
  }
}