#define M1DIR 7       // Right
#define M1PWM 9       // Right
#define M2DIR 8
#define M2PWM 10
#define RIGHT_MOTOR 0
#define LEFT_MOTOR  1


class MotorController {
  private:
    int m1_dir, m1_pwm;
    int m2_dir, m2_pwm;

  public:
    MotorController(int m1_dir, int m1_pwm, int m2_dir, int m2_pwm)
      : m1_dir(m1_dir), m1_pwm(m1_pwm), m2_dir(m2_dir), m2_pwm(m2_pwm) {}

    void begin() {
      pinMode(m1_dir, OUTPUT);
      pinMode(m1_pwm, OUTPUT);
      pinMode(m2_dir, OUTPUT);
      pinMode(m2_pwm, OUTPUT);
    }

    // motorIndex: 1 또는 2
    // speed: -400 ~ +400
    void setSpeed(int motorIndex, int speed) {
      // 범위 제한
      if (speed > 400) speed = 400;
      if (speed < -400) speed = -400;

      int dirPin, pwmPin;

      if (motorIndex == 0) {          // M1 : Right Motor
        dirPin = m1_dir;
        pwmPin = m1_pwm;
      } else if (motorIndex == 1) {   // M2 : Left Motor
        dirPin = m2_dir;
        pwmPin = m2_pwm;
      } else {
        return; // 잘못된 번호
      }

      if (speed == 0) {
        analogWrite(pwmPin, 0);
        return;
      }

      // 방향 처리
      if (speed > 0) {
        digitalWrite(dirPin, LOW);  // 정방향
      } else {
        digitalWrite(dirPin, HIGH); // 역방향
        speed = -speed;             // 절대값 변환
      }

      // PWM 값으로 변환
      int pwmValue = map(speed, 0, 400, 0, 255);
      analogWrite(pwmPin, pwmValue);
    }
};


// -----------------------------------------------
// MotorController 객체 하나만 생성
MotorController motor(M1DIR, M1PWM, M2DIR, M2PWM);

void setup() {
  Serial.begin(9600);
  motor.begin();
}

void loop() {
  Serial.println("Forward");
  motor.setSpeed(LEFT_MOTOR, 100);  // M1 정방향
  motor.setSpeed(RIGHT_MOTOR, 100);  // M2 정방향
  delay(1000);

  Serial.println("Stop");
  motor.setSpeed(LEFT_MOTOR, 0);
  motor.setSpeed(RIGHT_MOTOR, 0);
  delay(1000);

  Serial.println("Backward");
  motor.setSpeed(LEFT_MOTOR, -100); // M1 역방향
  motor.setSpeed(RIGHT_MOTOR, -100); // M2 역방향
  delay(1000);
}
