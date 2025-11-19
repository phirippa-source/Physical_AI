#define M1DIR     7
#define M2DIR     8
#define M1PWM     9
#define M2PWM     10

void setup() {
  Serial.begin(9600);
  pinMode(M1DIR, OUTPUT);
  pinMode(M2DIR, OUTPUT);
}

void loop() {
  // forward
  Serial.println("Forward");
  digitalWrite(M1DIR, LOW);
  digitalWrite(M2DIR, LOW);
  analogWrite(M1PWM, 64);
  analogWrite(M2PWM, 64);
  delay(1000);

  // stop
  Serial.println("Stop");
  analogWrite(M1PWM, 0);
  analogWrite(M2PWM, 0);
  delay(1000);

  // backward
  Serial.println("Backward");
  digitalWrite(M1DIR, HIGH);
  digitalWrite(M2DIR, HIGH);
  analogWrite(M1PWM, 64);
  analogWrite(M2PWM, 64);
  delay(1000);
}
