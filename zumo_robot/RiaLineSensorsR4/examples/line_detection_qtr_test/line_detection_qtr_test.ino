#include <QTRSensors.h>

QTRSensors qtr;
const uint8_t SensorCount = 6;
uint16_t sensorValues[SensorCount];
uint8_t sensorPins[SensorCount] = {4, A3, 11, A0, A2, 5};

void setup() {
  Serial.begin(115200);
  qtr.setTypeRC();      // RC 타입으로 설정
  // 센서 핀 등록
  qtr.setSensorPins(sensorPins, SensorCount);
  // 타임아웃(마이크로초). 
  // 환경에 따라 1500~4000us 정도에서 조정합니다.
  qtr.setTimeout(2500);
  Serial.println("Start reading RC sensors...");
}

void loop() {
  // 값이 클수록 보통 '더 어두움/검정'에 가까움
  qtr.read(sensorValues);

  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(sensorValues[i]);
    Serial.print(i == SensorCount - 1 ? '\n':'\t');
  }
  delay(50);
}
