#include <RiaLineSensorR4.h>
#define USER_SW   12

const int8_t EMITTER_PIN = 2; // IR LED 제어 핀
const int RIGHT_DIR_PIN = 7;
const int LEFT_DIR_PIN  = 8;
const int RIGHT_PWM_PIN = 9;
const int LEFT_PWM_PIN  = 10;

const uint8_t NUM_SENSORS = 6;
const uint8_t SENSOR_PINS[NUM_SENSORS] = {
  5, A2, A0, 11, A3, 4
};

// 라인 센서 객체
RiaLineSensorR4 lineSensor(SENSOR_PINS, NUM_SENSORS, EMITTER_PIN);

// 중앙 위치 (readLine()이 0 ~ (N-1)*1000 범위 반환한다고 가정)
// 5*1000/2 = 2500
const int16_t CENTER_POS = (NUM_SENSORS - 1) * 1000 / 2; 

// ===================== PID 설정 ==================
float Kp = 0.8f;
float Ki = 0.0f; 
float Kd = 0.1f;
float prevError = 0.0f;
float integral =  0.0f;
uint32_t prevTime = 0;
const int BASE_SPEED = 200;   // 직진 기준 속도 (0~255)
const int MAX_SPEED  = 255;   // PWM 최대값

// ================= 모터 제어 함수 ==================
void setMotor(int dirPin, int pwmPin, int speed) {
  // speed: -255 ~ +255
  bool forward = (speed >= 0);
  int pwm = abs(speed);
  if (pwm > 255) pwm = 255;
  digitalWrite(dirPin, forward ? LOW : HIGH);
  analogWrite(pwmPin, pwm);
}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  setMotor(LEFT_DIR_PIN, LEFT_PWM_PIN, leftSpeed);
  setMotor(RIGHT_DIR_PIN, RIGHT_PWM_PIN, rightSpeed);
}

void setup() {
  Serial.begin(115200);
  while (!Serial){;}// 시리얼 준비될 때까지 기다림
  pinMode(LEFT_DIR_PIN, OUTPUT);
  pinMode(RIGHT_DIR_PIN, OUTPUT);
  pinMode(LEFT_PWM_PIN, OUTPUT);
  pinMode(RIGHT_PWM_PIN, OUTPUT);
  pinMode(USER_SW, INPUT_PULLUP);

  lineSensor.begin();// 센서 초기화
  setMotorSpeeds(0, 0); // 모터 정지

  Serial.println("Calibration start...");
  Serial.println("로봇을 라인 위에 올려 놓고 SW를 누르세요");
  while(digitalRead(USER_SW)) {;}
  setMotorSpeeds(-100, 100);// 왼쪽 제자리 회전
  // 약 1초 정도 보정 (100회 * 10ms)
  for (int i = 0; i < 100; i++) {
    lineSensor.calibrate(1); // 한 번씩만 읽어서 min/max 갱신
    delay(10);
  }
  setMotorSpeeds(0, 0);delay(50);
  setMotorSpeeds(100, -100);  // 오른쪽 제자리 회전
  // 약 1초 정도 보정 (100회 * 10ms)
  for (int i = 0; i < 100; i++) {
    lineSensor.calibrate(1); // 한 번씩만 읽어서 min/max 갱신
    delay(10);
  }
  setMotorSpeeds(0, 0); // 모터 정지

  Serial.println("Calibration done.");
  Serial.println("USER SW를 누르면 라인트레이싱을 시작");
  while(digitalRead(USER_SW)) {;}
  prevTime = millis();
}

void loop() {
  // ---- 시간 계산 (dt) -----
  uint32_t now = millis();
  float dt = (prevTime == 0) ? 0.01f : (now - prevTime) / 1000.0f;  // 초 단위
  prevTime = now;
  // ----- 라인 위치 읽기 -----
  int16_t pos = lineSensor.readLine(false); // 검은 라인 기준, 0 ~ 5000 근처
  // PID 에러 계산: 중앙(CENTER_POS=2500)에서 얼마나 벗어났는지 
  int16_t error_raw = CENTER_POS - pos;   // -2500 ~ +2500
  float  error = (float)error_raw / (float)CENTER_POS; // 대략 -1.0 ~ +1.0
  // ----- PID 계산 -----
  integral += error * dt;
  float derivative = (dt > 0.0f) ? (error - prevError) / dt : 0.0f;
  prevError = error;
  float output = Kp * error + Ki * integral + Kd * derivative;
  // output은 "조향량"의 기본 재료 (왼/오른쪽 모터 속도 차이)
  // ----- PID 출력 → 모터 속도 변환 -----
  // output이 너무 크면 조향이 과격해지므로 적당히 스케일링
  float turn = output * 200.0f; // 필요하다면 이 숫자(200)를 줄이거나 늘려서 감도 조정
  float leftF = BASE_SPEED + turn;
  float rightF = BASE_SPEED - turn;
  int leftSpeed = (int)constrain(leftF, -MAX_SPEED, MAX_SPEED);
  int rightSpeed = (int)constrain(rightF, -MAX_SPEED, MAX_SPEED);
  // ----- 모터 구동 -----
  setMotorSpeeds(leftSpeed, rightSpeed);
  // 디버깅용 시리얼 출력 (원하면 주석 처리)
  // Serial.print("pos=");
  // Serial.print(pos);
  // Serial.print("\t error=");
  // Serial.print(error, 3);
  // Serial.print("\t out=");
  // Serial.print(output, 3);
  // Serial.print("\t L=");
  // Serial.print(leftSpeed);
  // Serial.print("\t R=");
  // Serial.println(rightSpeed);
  delay(10);  // 제어 주기 ≈ 100Hz (너무 작게 하면 시리얼 때문에 지연될 수도 있음)
}

