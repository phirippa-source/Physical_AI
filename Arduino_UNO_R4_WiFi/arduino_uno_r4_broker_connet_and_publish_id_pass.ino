// 사용자 ID와 Password를 가지고 MQTT Broker에 접속하는 환경
#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
int status = WL_IDLE_STATUS;

// --------------------- WiFi 정보 -------------------------
char ssid[] = "Ria2G";
char password[] = "730124go";

// --------------------- MQTT 브로커 정보 -------------------
const char broker[] = "192.168.211.47";   // MQTT 서버 주소
int        port     = 1883;

// --------------------- MQTT 인증 정보 ---------------------
const char mqttUser[] = "ship";     // MQTT ID
const char mqttPass[] = "1234"; // MQTT Password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // 공유기 연결 시도
  Serial.print("Connecting to " + String(ssid));
  while (status != WL_CONNECTED) {
    Serial.print(".");
    status = WiFi.begin(ssid, password);
    delay(1000);  // 재시도 간격
  }
  Serial.println("\nWiFi 연결 성공!");
  Serial.println("Arduino Board의 IP 주소: " + WiFi.localIP().toString());

  // ---------- MQTT 설정 ----------
  mqttClient.setUsernamePassword(mqttUser, mqttPass);

  Serial.print("MQTT 브로커 연결중...");
  if (!mqttClient.connect(broker, port)) {
    Serial.print("실패! 오류 코드: ");
    Serial.println(mqttClient.connectError());
    delay(1000);
  }
  Serial.println("성공!");
  mqttClient.setKeepAliveInterval(60); // 옵션
}

void loop() {
  mqttClient.poll(); // 연결 유지 필수

  Serial.println("Publish 메세지 전송!");
  mqttClient.beginMessage("test/topic");
  mqttClient.print("Hello from UNO R4 WiFi + MQTT Auth");
  mqttClient.endMessage();

  delay(2000);
}
