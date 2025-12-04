// 모터 핀 (ZumoShield 기본)
// 오른쪽 모터: DIR=7, PWM=9,  왼쪽  모터: DIR=8, PWM=10
const int RIGHT_DIR_PIN = 7;
const int LEFT_DIR_PIN  = 8;
const int RIGHT_PWM_PIN = 9;
const int LEFT_PWM_PIN  = 10;

void setMotor(int dirPin, int pwmPin, int speed) {
  // speed: -255 ~ +255
  bool forward = (speed >= 0);
  int pwm = abs(speed);
  if (pwm > 255) pwm = 255;

  // ZumoShield 기준: LOW = forward, HIGH = reverse 인 경우가 많음
  digitalWrite(dirPin, forward ? LOW : HIGH);
  analogWrite(pwmPin, pwm);
}
void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  setMotor(LEFT_DIR_PIN, LEFT_PWM_PIN, leftSpeed);
  setMotor(RIGHT_DIR_PIN, RIGHT_PWM_PIN, rightSpeed);
}
void setup() {
  pinMode(LEFT_DIR_PIN, OUTPUT);
  pinMode(RIGHT_DIR_PIN, OUTPUT);
  pinMode(LEFT_PWM_PIN, OUTPUT);
  pinMode(RIGHT_PWM_PIN, OUTPUT);
  setMotorSpeeds(0, 0); 
}
void loop(){
  setMotorSpeeds(-100, 100);    // 왼쪽 제자리 회전
  delay(300);
  setMotorSpeeds(0, 0);         // 모터 정지
  delay(200);
  setMotorSpeeds(100, -100);    // 오른쪽 제자리 회전
  delay(300);
  setMotorSpeeds(0, 0);         // 모터 정지
  delay(1000);
}
