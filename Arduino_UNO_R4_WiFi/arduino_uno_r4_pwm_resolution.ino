/*
Both UNO R4 boards (Minima and WiFi) can increase the resolution up to 12 bits using analogWriteResolution( )
*/

void setup() {
  // PWM 분해능을 12비트로 설정 → 0~4095
  analogWriteResolution(12); 
  //pinMode(9, OUTPUT);
}

void loop() {
  analogWrite(9, 4095);
  delay(1000);

  // 대략 50% duty
  analogWrite(9, 2047);
  delay(1000);
}
