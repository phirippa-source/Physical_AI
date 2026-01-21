#define USERPUSHBUTTON  12

void setup(){
  Serial.begin(115200);
  // User push button 핀을 (내부 풀업 저항을 이용하여) PULLUP으로 설정
  // 따라서 스위치를 누르지 않은 경우 1, 스위치를 누르면 0
  pinMode(USERPUSHBUTTON, INPUT_PULLUP);
}

void loop() {
  int btn_state = digitalRead(USERPUSHBUTTON);
  Serial.println(btn_state);
  delay(50);
}
