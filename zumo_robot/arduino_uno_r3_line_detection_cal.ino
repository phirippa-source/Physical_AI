#include <Wire.h>
#include <ZumoShield.h>
#define NUM_SENSORS 6
ZumoMotors motors;
ZumoReflectanceSensorArray reflectanceSensors;
unsigned int sensorValues[NUM_SENSORS];

void setup()
{
  Serial.begin(9600);
  reflectanceSensors.init();
  Serial.println("Starting auto-calibration...");
  // ------------- 라인 검출 센서 캘리브레이션 --------------
  // 왼쪽 회전 40회
  for (uint16_t i = 0; i < 40; i++) {
    motors.setSpeeds(-150, 150);
    reflectanceSensors.calibrate();
    delay(20);
  }

  // 오른쪽 회전 40회
  for (uint16_t i = 0; i < 40; i++) {
    motors.setSpeeds(150, -150);
    reflectanceSensors.calibrate();
    delay(20);
  }

  // 모터 정지
  motors.setSpeeds(0, 0);
  Serial.println("Calibration complete!");
}

void loop() {
  // 캘리브레이션된 값 읽기 (0~1000)
  reflectanceSensors.readCalibrated(sensorValues);

  // 센서값 출력
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println();
  delay(100);
}
