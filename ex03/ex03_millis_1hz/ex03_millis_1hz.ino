// 作业6：使用millis()函数实现1Hz定时闪烁（亮500ms、灭500ms）
const int onboardLED = 2;        // ESP32板载LED引脚
bool currentLEDStatus = LOW;     // LED当前状态
unsigned long previousRecordTime = 0;  // 上一次状态切换的时间
const long blinkDuration = 500;  // 状态保持时长

void setup() {
  pinMode(onboardLED, OUTPUT);   // 设置LED引脚为输出模式
  Serial.begin(115200);          // 初始化串口通信
}

void loop() {
  unsigned long currentTime = millis();  // 获取当前系统时间

  // 非阻塞延时判断，不影响其他程序运行
  if (currentTime - previousRecordTime >= blinkDuration) {
    previousRecordTime = currentTime;    // 更新时间记录
    currentLEDStatus = !currentLEDStatus;// 翻转LED状态
    digitalWrite(onboardLED, currentLEDStatus);  // 控制LED亮灭
    
    // 串口打印LED当前状态
    Serial.print("LED Status: ");
    Serial.println(currentLEDStatus ? "Light ON" : "Light OFF");
  }
}