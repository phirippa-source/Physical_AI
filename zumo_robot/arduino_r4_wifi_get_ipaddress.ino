#include <WiFiS3.h>
char ssid[] = "SSID_NAME";       // 연결할 WiFi 이름
char password[] = "PASSWORD";    // WiFi 비밀번호
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  // WiFi 모듈 확인
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi 모듈을 찾을 수 없습니다!");
    while (true);
  }
  // 공유기 연결 시도
  Serial.print("Connecting to " + String(ssid));
  while (status != WL_CONNECTED) {
    Serial.print(".");
    status = WiFi.begin(ssid, password);
    delay(1000);  // 재시도 간격
  }
  Serial.println("\nWiFi 연결 성공!");
  Serial.println("Arduino Board의 IP 주소: " + WiFi.localIP().toString());
}

void loop() {
  // 주기적으로 RSSI 출력
  Serial.print("신호 세기(RSSI): ");
  Serial.println(WiFi.RSSI());
  delay(2000);
}
