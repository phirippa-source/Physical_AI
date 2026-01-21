#include <RiaLineSensorR4.h>

// ZumoShield 기본 핀 배치 (docs 기준)
// 센서: 5, A2, A0, 11, A3, 4  :contentReference[oaicite:4]{index=4}


const uint8_t SENSOR_PINS[6] = {
  5, A2, A0, 11, A3, 4
};

// LEDON 점퍼가 UNO 쪽(= D2)로 되어 있다고 가정
RiaLineSensorR4 lineSensor(SENSOR_PINS, 6, 2);

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  lineSensor.begin();

  Serial.println("Calibrating... 로봇을 흰 바탕과 검은 라인 위로 천천히 흔들어 주세요.");

  // 약 2초 동안 보정 (100회 * 20ms)
  for (int i = 0; i < 100; i++) {
    lineSensor.calibrate(1); // 한 번씩만 읽어서 min/max 갱신
    delay(20);
  }

  Serial.println("Calibration done.");
}

void loop() {
  uint16_t raw[6];
  lineSensor.readRaw(raw);

  Serial.print("raw:\t");
  for (int i = 0; i < 6; i++) {
    Serial.print(raw[i]);
    Serial.print('\t');
  }

  int16_t pos = lineSensor.readLine(false); // false: 검은 선 기준
  Serial.print(" pos=");
  Serial.println(pos);

  delay(100);
}
