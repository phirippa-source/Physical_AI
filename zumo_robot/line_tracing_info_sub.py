import csv
import os
from datetime import datetime

import paho.mqtt.client as mqtt

# ===== MQTT 설정 =====
MQTT_BROKER = "192.168.0.15"  # 브로커 IP
MQTT_PORT   = 1883
MQTT_TOPIC  = "ria/ship/line_logger"

MQTT_USER   = "ship"
MQTT_PASS   = "1234"

# ===== CSV 파일 설정 =====
# 실행 시각을 파일 이름에 넣어서 중복 방지
timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
CSV_FILENAME = f"ria_line_log_{timestamp}.csv"

# CSV 헤더 (UNO R4가 보내는 순서에 맞춰 작성)
CSV_HEADER = [
    "time_ms",
    "s0", "s1", "s2", "s3", "s4", "s5",
    "error",
    "pid_output",
    "left_speed",
    "right_speed",
]

# 파일 열기 (append 모드, 없으면 생성)
csv_file = open(CSV_FILENAME, mode="a", newline="", encoding="utf-8")
csv_writer = csv.writer(csv_file)

# 헤더가 비어 있다면 헤더 쓰기
if os.path.getsize(CSV_FILENAME) == 0:
    csv_writer.writerow(CSV_HEADER)
    csv_file.flush()


# ===== MQTT 콜백 함수들 =====
def on_connect(client, userdata, flags, reason_code, properties=None):
    print("MQTT 연결 결과:", reason_code)
    if reason_code == 0:
        print("MQTT 브로커 연결 성공!")
        client.subscribe(MQTT_TOPIC)
        print(f"토픽 구독: {MQTT_TOPIC}")
    else:
        print("MQTT 브로커 연결 실패")


def on_message(client, userdata, msg):
    try:
        payload_str = msg.payload.decode("utf-8").strip()
        print("수신:", payload_str)

        # CSV 형태이므로 ',' 기준으로 split
        parts = payload_str.split(",")

        # 기대하는 값 개수: 11개
        if len(parts) != len(CSV_HEADER):
            print(f"⚠ 필드 개수 불일치: {len(parts)}개 (기대: {len(CSV_HEADER)}개)")
            return

        # 그대로 행으로 기록
        csv_writer.writerow(parts)
        csv_file.flush()

    except Exception as e:
        print("메시지 처리 중 오류:", e)


# ===== MQTT 클라이언트 설정 및 실행 =====
def main():
    client = mqtt.Client(
        client_id="ria_line_logger_pc"    # PC 쪽 클라이언트 ID (아두이노와 다르게 지정)
    )

    # 인증 정보 설정
    client.username_pw_set(MQTT_USER, MQTT_PASS)

    # 콜백 함수 등록
    client.on_connect = on_connect
    client.on_message = on_message

    # 브로커 접속
    print(f"브로커 {MQTT_BROKER}:{MQTT_PORT} 에 연결 시도 중...")
    client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)

    print(f"CSV 로깅 시작: {CSV_FILENAME}")
    print("Ctrl+C 로 종료할 수 있습니다.")

    try:
        # 메시지 수신 루프 (무한 루프)
        client.loop_forever()
    except KeyboardInterrupt:
        print("\n사용자 중단, 종료합니다.")
    finally:
        csv_file.close()
        client.disconnect()


if __name__ == "__main__":
    main()
