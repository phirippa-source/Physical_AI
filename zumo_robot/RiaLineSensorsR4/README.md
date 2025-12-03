# RiaLineSensorR4

Arduino UNO R4 WiFi 보드에서  
**Pololu Zumo Reflectance Sensor Array (Zumo Shield용 라인 센서)** 를 사용하기 위한 전용 라이브러리입니다.

기존 `ZumoShield` / `QTRSensors` 라이브러리는 AVR(UNO R3) 타이밍/타이머에 맞춰 설계되어 있어  
UNO R4(renesas_uno) 환경에서는 센서 값 스케일이 깨지거나 컴파일 오류가 발생할 수 있습니다.

`RiaLineSensorR4`는 UNO R4에서 동작하도록  
RC 방식(Line Sensor 충전/방전 시간 측정)을 직접 구현한 경량 라이브러리입니다.

---

## Features

- Arduino UNO R4 WiFi에서 동작하도록 설계
- Pololu Zumo Reflectance Sensor Array(QTR-RC 타입) 지원
- 원시값(충전→방전 시간) 읽기
- 최소/최대값 자동 보정 (Calibration)
- 0~1000 범위로 정규화된 센서 값
- 센서 배열 기준 라인 위치 계산 (0 ~ (N-1)\*1000)

---

## Hardware

- **Controller**: Arduino UNO R4 WiFi  
- **Sensor**: Zumo Reflectance Sensor Array (Pololu, Zumo Shield용 RC 타입)  
- **보드 조합 예시**: Zumo Shield for Arduino + Zumo Reflectance Sensor Array + UNO R4 WiFi

예시 핀 배치(기본값):

```text
센서 핀: 5, A2, A0, 11, A3, 4   (총 6개)
IR LED 제어(LEDON 점퍼): D2 에 연결되어 있다고 가정



