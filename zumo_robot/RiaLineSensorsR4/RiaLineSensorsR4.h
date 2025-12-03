#ifndef RIA_LINE_SENSOR_R4_H
#define RIA_LINE_SENSOR_R4_H

#include <Arduino.h>

class RiaLineSensorR4 {
public:
    static const uint8_t MAX_SENSORS = 6;

    // pins: 센서가 연결된 핀 배열
    // numSensors: 센서 개수 (Zumo 기본은 6)
    // emitterPin: IR LED 제어 핀 (예: 2, 없으면 -1)
    RiaLineSensorR4(const uint8_t *pins,
                     uint8_t numSensors,
                     int8_t emitterPin = -1);

    void begin();

    // 원시값 읽기 (0 ~ timeoutMicros, 값이 클수록 어두움)
    void readRaw(uint16_t *values, uint16_t timeoutMicros = 2500);

    // 보정값 초기화
    void resetCalibration();

    // 여러 번 읽어서 min/max 갱신 (로봇을 흰/검정 위로 왔다갔다 시키면서 호출)
    void calibrate(uint8_t iterations = 10, uint16_t timeoutMicros = 2500);

    // 0~1000 범위로 정규화 (0=흰색, 1000=검정)
    void readCalibrated(uint16_t *values, uint16_t timeoutMicros = 2500);

    // 라인 위치 계산 (0 ~ (numSensors-1)*1000)
    // whiteLine == false : 검은 선 (기본)
    // whiteLine == true  : 흰 선
    int16_t readLine(bool whiteLine = false, uint16_t timeoutMicros = 2500);

private:
    uint8_t  _numSensors;
    uint8_t  _pins[MAX_SENSORS];
    int8_t   _emitterPin;

    bool     _calibrated;
    uint16_t _calibMin[MAX_SENSORS];
    uint16_t _calibMax[MAX_SENSORS];

    int32_t  _lastPosition;
};

#endif // RIA_LINE_SENSOR_R4_H
