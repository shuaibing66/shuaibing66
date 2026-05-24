
const int boardLed = 2;

void setup() {
  // 启动串口调试，波特率115200
  Serial.begin(115200);
  // 设置LED引脚为输出模式
  pinMode(boardLed, OUTPUT);
}

void loop() {
  // 点亮LED并打印状态
  digitalWrite(boardLed, HIGH);
  Serial.println("Status: LED is ON");
  delay(1000);
   // 熄灭LED并打印状态
  digitalWrite(boardLed, LOW);
  Serial.println("Status: LED is OFF");
  delay(1000);
}
