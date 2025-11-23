#include <SoftwareSerial.h>

#define BT_RXD 2    // Bluetooth RX 핀
#define BT_TXD 3    // Bluetooth TX 핀
#define JOY_X A0    // 조이스틱 X축
#define JOY_Y A1    // 조이스틱 Y축
#define BUTTON_PIN 8  // 조이스틱 버튼 핀

SoftwareSerial hc06(BT_RXD, BT_TXD); // Bluetooth 시리얼 통신 객체 생성

int joyX = 0;  // X축 값
int joyY = 0;  // Y축 값
int buttonState = 0;  // 버튼 상태 (클락션)

void setup() {
  Serial.begin(9600);
  hc06.begin(9600);  // HC-06 Bluetooth 모듈과 통신 시작
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // 버튼 핀 설정
}

void loop() {
  // 조이스틱 아날로그 값 읽기
  joyX = analogRead(JOY_X);  // X축 (0 ~ 1023)
  joyY = analogRead(JOY_Y);  // Y축 (0 ~ 1023)

  // 조이스틱 버튼 상태 읽기
  buttonState = digitalRead(BUTTON_PIN);  // 버튼 상태 (버튼 눌림 상태는 LOW)

  // X, Y축 값을 0~1023 범위에서 세밀하게 변환 (최소값 및 최대값을 설정하여 제어)
  int normalizedX = map(joyX, 0, 1023, -500, 500); // -500 (왼쪽) ~ 500 (오른쪽)
  int normalizedY = map(joyY, 0, 1023, -500, 500); // -500 (후진) ~ 500 (전진)

  // Bluetooth로 데이터 전송
  if (buttonState == LOW) {  // 버튼이 눌리면 클락션 신호 보내기
    hc06.print("-"); // 경적 (클락션)
  } else {
    // Y축에 따른 전진 / 후진
    if (normalizedY < -200) {
      // 후진
      hc06.print("0");  // 후진
      hc06.print(",");  // 구분
      hc06.print(abs(normalizedY));  // 후진 속도
    } else if (normalizedY > 200) {
      // 전진
      hc06.print("1");  // 전진
      hc06.print(",");  // 구분
      hc06.print(abs(normalizedY));  // 전진 속도
    } else {
      // 정지
      hc06.print("500");  // 정지
    }

    // X축에 따른 회전 (좌회전 / 우회전)
    if (normalizedX < -100) {
      hc06.print(",");  // 구분
      hc06.print("*");  // 좌회전
      hc06.print(",");  // 구분
      hc06.print(abs(normalizedX));  // 회전 강도
    } else if (normalizedX > 100) {
      hc06.print(",");  // 구분
      hc06.print("/");  // 우회전
      hc06.print(",");  // 구분
      hc06.print(abs(normalizedX));  // 회전 강도
    }
  }

  delay(100);  // Bluetooth 통신이 너무 빨리 일어나지 않도록 잠시 대기
}
