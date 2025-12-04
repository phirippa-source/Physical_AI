#include <WiFiS3.h>            // UNO R4 WiFi 전용 Wi-Fi 라이브러리
#include <ArduinoMqttClient.h> // MQTT 클라이언트
#include <RiaLineSensorR4.h>   // 너가 만든 라인 센서 라이브러리

// ===================== Wi-Fi / MQTT 설정 =====================

// Wi-Fi 접속 정보
const char WIFI_SSID[] = "Ria2G";
const char WIFI_PASS[] = "730124go";

// MQTT 브로커 (mosquitto) 주소
const char MQTT_BROKER[] = "192.168.0.15";   // <- 여기 IP를 네 브로커 IP로 변경
const int  MQTT_PORT    = 1883;

const char MQTT_USER[]  = "ship";
const char MQTT_PASSW[] = "1234";

// MQTT 토픽
const char MQTT_TOPIC[] = "ship/line_logger";

// WiFi / MQTT 클라이언트 객체
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// ===================== 라인 센서 / 모터 / PID 설정 =====================

// Zumo Reflectance Sensor Array 핀 (예시: ZumoShield 기본)
const uint8_t NUM_SENSORS = 6;
const uint8_t SENSOR_PINS[NUM_SENSORS] = {
  5, A2, A0, 11, A3, 4
};

// IR LED 제어 핀 (LEDON 점퍼가 D2에 연결되어 있다고 가정)
const int8_t EMITTER_PIN = 2;

// 모터 핀 (ZumoShield 기본)
// 오른쪽 모터: DIR=D7, PWM=D9
// 왼쪽  모터: DIR=D8, PWM=D10
const int RIGHT_DIR_PIN = 7;
const int LEFT_DIR_PIN  = 8;
const int RIGHT_PWM_PIN = 9;
const int LEFT_PWM_PIN  = 10;

// 라인 센서 객체
RiaLineSensorR4 lineSensor(SENSOR_PINS, NUM_SENSORS, EMITTER_PIN);

// readLine()이 0 ~ (N-1)*1000을 반환한다고 가정 → 센터는 2500
const int16_t CENTER_POS = (NUM_SENSORS - 1) * 1000 / 2; // 2500

// PID 계수
float Kp = 0.76f;      // 0.8
float Ki = 0.004f;      // 0.0
float Kd = 0.17f;      // 0.1

// PID 상태 변수
float prevError   = 0.0f;
float integral    = 0.0f;
uint32_t prevTime = 0;

// 모터 속도 설정
const int BASE_SPEED = 200;  // 직진 기본 속도 (0~255)
const int MAX_SPEED  = 255;

// ====================================================
// 모터 제어 함수
// ====================================================
void setMotor(int dirPin, int pwmPin, int speed)
{
  // speed: -255 ~ +255
  bool forward = (speed >= 0);
  int pwm = abs(speed);
  if (pwm > 255) pwm = 255;

  // ZumoShield 기준: LOW = forward, HIGH = reverse 인 경우가 많음
  digitalWrite(dirPin, forward ? LOW : HIGH);
  analogWrite(pwmPin, pwm);
}

void setMotorSpeeds(int leftSpeed, int rightSpeed)
{
  setMotor(LEFT_DIR_PIN,  LEFT_PWM_PIN,  leftSpeed);
  setMotor(RIGHT_DIR_PIN, RIGHT_PWM_PIN, rightSpeed);
}

// ====================================================
// Wi-Fi / MQTT 연결 함수
// ====================================================

void connectWiFi()
{
  Serial.print("WiFi 연결 중: ");
  Serial.println(WIFI_SSID);

  int status = WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (status != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    status = WiFi.status();
  }

  Serial.println("\nWiFi 연결 완료!");
  Serial.print("IP 주소: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT()
{
  Serial.print("MQTT 브로커 연결 중: ");
  Serial.print(MQTT_BROKER);
  Serial.print(":");
  Serial.println(MQTT_PORT);
  mqttClient.setUsernamePassword(MQTT_USER, MQTT_PASSW);

  while (!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
    Serial.print("MQTT 연결 실패, 에러 코드 = ");
    Serial.println(mqttClient.connectError());
    Serial.println("3초 후 재시도...");
    delay(3000);
  }

  Serial.println("MQTT 연결 완료!");
}

// ====================================================
// setup()
// ====================================================
void setup()
{
  Serial.begin(115200);
  while (!Serial) { ; }

  // 모터 핀 설정
  pinMode(LEFT_DIR_PIN,  OUTPUT);
  pinMode(RIGHT_DIR_PIN, OUTPUT);
  pinMode(LEFT_PWM_PIN,  OUTPUT);
  pinMode(RIGHT_PWM_PIN, OUTPUT);

  // 센서 초기화
  lineSensor.begin();

  // 모터 정지
  setMotorSpeeds(0, 0);

  Serial.println("=== RiaLineSensorR4 + PID + MQTT Logger (UNO R4 WiFi) ===");

  // Wi-Fi / MQTT 연결
  connectWiFi();
  connectMQTT();

  // MQTT keep-alive 설정 (선택)
  mqttClient.setKeepAliveInterval(60); // 60초

  // 센서 보정
  Serial.println("Calibration start...");
  Serial.println("로봇을 손으로 잡고 흰 바탕과 검은 라인 위를 여러 방향으로 천천히 움직여 주세요.");

  for (int i = 0; i < 100; i++) {
    lineSensor.calibrate(1);
    delay(20);
  }

  Serial.println("Calibration done.");
  Serial.println("라인트레이싱 + MQTT 로깅을 시작합니다.");

  prevTime = millis();
}

// ====================================================
// loop()
// ====================================================
void loop()
{
  // Wi-Fi / MQTT 연결 상태 체크
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi 끊김, 재연결 시도...");
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    Serial.println("MQTT 끊김, 재연결 시도...");
    connectMQTT();
  }

  mqttClient.poll();  // 내부 MQTT 유지용

  // ----- 시간 계산 (dt) -----
  uint32_t now = millis();
  float dt = (prevTime == 0) ? 0.01f : (now - prevTime) / 1000.0f;
  prevTime = now;

  // ----- 센서 읽기 -----
  uint16_t calVals[RiaLineSensorR4::MAX_SENSORS];
  lineSensor.readCalibrated(calVals);  // 0~1000 범위 (흰~검)

  int16_t pos = lineSensor.readLine(false);  // 0 ~ 5000

  // ----- PID 에러 계산 -----
  int16_t error_raw = CENTER_POS - pos;                 // -2500 ~ +2500
  float   error     = (float)error_raw / (float)CENTER_POS; // -1.0 ~ +1.0

  // ----- PID 계산 -----
  integral += error * dt;
  float derivative = (dt > 0.0f) ? (error - prevError) / dt : 0.0f;
  prevError = error;

  float pidOutput = Kp * error + Ki * integral + Kd * derivative;

  // ----- 모터 속도 계산 -----
  float turn = pidOutput * 200.0f;   // 조향량 스케일

  float leftF  = BASE_SPEED + turn;
  float rightF = BASE_SPEED - turn;

  int leftSpeed  = (int)constrain(leftF,  -MAX_SPEED, MAX_SPEED);
  int rightSpeed = (int)constrain(rightF, -MAX_SPEED, MAX_SPEED);

  setMotorSpeeds(leftSpeed, rightSpeed);

  // ----- MQTT로 로깅 데이터 전송 -----
  // 포맷:
  // time_ms,s0,s1,s2,s3,s4,s5,error,pidOutput,leftSpeed,rightSpeed
  String payload;
  payload.reserve(128);  // 메모리 미리 확보

  payload += now;  // time_ms
  payload += ",";

  for (int i = 0; i < NUM_SENSORS; i++) {
    payload += calVals[i];  // 보정된 센서 값 (0~1000)
    payload += (i < NUM_SENSORS - 1) ? "," : ",";
  }

  payload += String(error, 4);     // error (float, -1.0 ~ +1.0)
  payload += ",";
  payload += String(pidOutput, 4); // PID output
  payload += ",";
  payload += leftSpeed;            // 모터 속도
  payload += ",";
  payload += rightSpeed;

  // MQTT publish
  mqttClient.beginMessage(MQTT_TOPIC);
  mqttClient.print(payload);
  mqttClient.endMessage();

  // 디버깅용 시리얼 출력 (원하면 꺼도 됨)
  Serial.println(payload);

  delay(10);  // 제어 주기 약 100Hz
}
