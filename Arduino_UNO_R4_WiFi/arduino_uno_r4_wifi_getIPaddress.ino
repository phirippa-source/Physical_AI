#include <WiFiS3.h>            // UNO R4 WiFi 전용 Wi-Fi 라이브러리

IPAddress getLocalIP(unsigned long timeout_ms = 3000){
  IPAddress ip(0,0,0,0);
  unsigned long int start_time = millis();

  // 타임 아웃(timeout_ms) 시간 (이내)까지만  (DHCP 프로토콜로) IP 주소를 받기 위해 대기
  while (millis() - start_time < timeout_ms) {
    ip = WiFi.localIP();
    if (ip[0] != 0 ) return ip;
    delay(100);
  }
  return IPAddress(0,0,0,0);  //WiFi 공유기와 무선 연결은 되었으나 주어진 시간 이내에 IP 주소를 받아오지 못함.
}

bool connectWiFi(char *SSID, char *WIFI_PASSWD) {
  // WiFi 공유기에 연결 시도
  WiFi.begin(SSID, WIFI_PASSWD);

  while( WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  return true;
}

void setup() {
  // Wi-Fi 접속 정보
  char SSID[] = "Ria2G";
  char WIFI_PASSWD[] = "730124go";

  Serial.begin(115200);
  while (!Serial) { ; }

  Serial.println("공유기(" + String(SSID) + ")에 접속을 시도합니다."); 
  if ( connectWiFi(SSID, WIFI_PASSWD) ) {
    Serial.println("WiFi connected!");
    Serial.print("Arduino UNO R4 WiFi 보드의 IP 주소: ");
    Serial.println( getLocalIP(1000) );
  } else {
    Serial.println("\nE: 주어진 시간 내에 WiFi 공유기와 접속하는데 실패함.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
