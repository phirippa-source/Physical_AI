/*
The code below performs the same behavior on both the Arduino Uno R3 and the Arduino Uno R4 (WiFi)
In the setup( ) function, pin 9 is configured as an output.
In the loop( ) function, the following operations are repeatedly executed using the analogWrite( )
Output a signal with a 100% duty cycle (HIGH period ratio) on pin 9 for 1 second
Output a signal with a 50% duty cycle (HIGH period ratio) on pin 9 for 1 second
*/

void setup() {
  pinMode(9, OUTPUT);
}

void loop() {
  analogWrite(9, 255);
  delay(1000);
  analogWrite(9, 127);
  delay(1000);
}
