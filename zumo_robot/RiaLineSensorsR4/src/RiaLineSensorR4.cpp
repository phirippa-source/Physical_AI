#include "RiaLineSensorR4.h"

RiaLineSensorR4::RiaLineSensorR4(const uint8_t *pins,
                                   uint8_t numSensors,
                                   int8_t emitterPin)
: _numSensors(numSensors),
  _emitterPin(emitterPin),
  _calibrated(false),
  _lastPosition(0)
{
    if (_numSensors > MAX_SENSORS) {
        _numSensors = MAX_SENSORS;
    }

    for (uint8_t i = 0; i < _numSensors; i++) {
        _pins[i] = pins[i];
        _calibMin[i] = 0xFFFF;
        _calibMax[i] = 0;
    }

    // 초기 lastPosition은 센서 중앙으로
    if (_numSensors > 0) {
        _lastPosition = (_numSensors - 1) * 1000L / 2;
    }
}

void RiaLineSensorR4::begin()
{
    if (_emitterPin >= 0) {
        pinMode(_emitterPin, OUTPUT);
        digitalWrite(_emitterPin, HIGH); // 항상 ON (전류 아끼려면 필요할 때만 켜도 됨)
    }

    for (uint8_t i = 0; i < _numSensors; i++) {
        pinMode(_pins[i], INPUT);
    }
}

void RiaLineSensorR4::readRaw(uint16_t *values, uint16_t timeoutMicros)
{
    if (_emitterPin >= 0) {
        digitalWrite(_emitterPin, HIGH);
        delayMicroseconds(10);
    }

    // 1) 모두 HIGH 출력으로 설정해서 충전
    for (uint8_t i = 0; i < _numSensors; i++) {
        pinMode(_pins[i], OUTPUT);
        digitalWrite(_pins[i], HIGH);
    }

    delayMicroseconds(10); // 충전 시간

    // 2) 기본값은 timeout으로 초기화
    for (uint8_t i = 0; i < _numSensors; i++) {
        values[i] = timeoutMicros;
    }

    // 3) 입력으로 전환 + 방전 시간 측정
    for (uint8_t i = 0; i < _numSensors; i++) {
        pinMode(_pins[i], INPUT);
    }

    uint32_t start = micros();

    while (true) {
        uint32_t now = micros();
        uint32_t elapsed = now - start;

        if (elapsed >= timeoutMicros) {
            break;
        }

        for (uint8_t i = 0; i < _numSensors; i++) {
            // 아직 시간 기록 안 된 센서만 체크
            if (values[i] == timeoutMicros) {
                if (digitalRead(_pins[i]) == LOW) {
                    values[i] = (uint16_t)elapsed;
                }
            }
        }
    }

    // 필요하면 여기서 emitter를 끄고, 읽을 때만 켜도록 변경 가능
    // if (_emitterPin >= 0) {
    //     digitalWrite(_emitterPin, LOW);
    // }
}

void RiaLineSensorR4::resetCalibration()
{
    _calibrated = false;
    for (uint8_t i = 0; i < _numSensors; i++) {
        _calibMin[i] = 0xFFFF;
        _calibMax[i] = 0;
    }
}

void RiaLineSensorR4::calibrate(uint8_t iterations, uint16_t timeoutMicros)
{
    uint16_t raw[MAX_SENSORS];

    for (uint8_t n = 0; n < iterations; n++) {
        readRaw(raw, timeoutMicros);

        for (uint8_t i = 0; i < _numSensors; i++) {
            uint16_t v = raw[i];

            if (!_calibrated) {
                _calibMin[i] = v;
                _calibMax[i] = v;
            } else {
                if (v < _calibMin[i]) _calibMin[i] = v;
                if (v > _calibMax[i]) _calibMax[i] = v;
            }
        }

        _calibrated = true;
        delay(5);
    }
}

void RiaLineSensorR4::readCalibrated(uint16_t *values, uint16_t timeoutMicros)
{
    uint16_t raw[MAX_SENSORS];
    readRaw(raw, timeoutMicros);

    for (uint8_t i = 0; i < _numSensors; i++) {
        uint16_t v    = raw[i];
        uint16_t minv = _calibMin[i];
        uint16_t maxv = _calibMax[i];

        if (!_calibrated || maxv <= minv) {
            // 보정 정보가 없으면 0으로
            values[i] = 0;
        } else {
            // raw: 작을수록 흰색, 클수록 검정
            int32_t num = (int32_t)v - (int32_t)minv;
            if (num < 0) num = 0;
            uint16_t norm = (uint16_t)((num * 1000L) / (maxv - minv));
            if (norm > 1000) norm = 1000;
            values[i] = norm;  // 0 ~ 1000 (흰색 ~ 검정)
        }
    }
}

int16_t RiaLineSensorR4::readLine(bool whiteLine, uint16_t timeoutMicros)
{
    uint16_t cal[MAX_SENSORS];
    readCalibrated(cal, timeoutMicros);

    uint32_t weightedSum = 0;
    uint32_t sum = 0;

    for (uint8_t i = 0; i < _numSensors; i++) {
        uint16_t v = cal[i];

        // whiteLine 모드면 흰색을 1000으로 뒤집기
        if (whiteLine) {
            v = 1000 - v;
        }

        // 너무 작은 값은 노이즈로 무시 (임계값 50 정도)
        if (v < 50) continue;

        weightedSum += (uint32_t)v * (i * 1000UL);
        sum += v;
    }

    if (sum == 0) {
        // 라인 못 찾았으면 마지막 위치 반환
        return (int16_t)_lastPosition;
    }

    _lastPosition = (int32_t)(weightedSum / sum);
    return (int16_t)_lastPosition;
}
