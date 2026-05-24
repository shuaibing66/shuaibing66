#include <WiFi.h>
#include <WebServer.h>

// -------------------------- 系统参数配置 --------------------------
const char* wifiName = "QAQ";
const char* wifiPwd = "94pnnxzmr6gpifj";
const int alarmLedPin = 2;                // 报警指示灯引脚
const int touchSensorPin = T0;            // 触摸检测引脚
const int touchTriggerLevel = 30;         // 触摸触发阈值
// ------------------------------------------------------------------

WebServer webServer(80);

// 系统全局状态
bool systemEnabled = false;    // 系统是否布防
bool alarmTriggered = false;    // 是否触发报警
unsigned long previousFlashTime = 0;
const int flashSpeed = 100;     // 报警闪烁速度

// 首页页面处理
void showHomePage() {
  String sysState = systemEnabled ? "已布防" : "已撤防";
  String alarmState = alarmTriggered ? " 正在报警！" : "系统正常";
  
  String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 智能安防报警系统</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    .btn { padding: 15px 30px; font-size: 20px; margin: 10px; border: none; border-radius: 5px; cursor: pointer; }
    .arm-btn { background-color: #dc3545; color: white; }
    .disarm-btn { background-color: #28a745; color: white; }
    .status { font-size: 24px; margin: 20px 0; }
    .alarm { color: red; font-weight: bold; }
  </style>
</head>
<body>
  <h1>智能安防报警控制系统</h1>
  <div class="status">系统状态: <span id="status">)rawliteral" + sysState + R"rawliteral(</span></div>
  <div class="status alarm" id="alarmStatus">)rawliteral" + alarmState + R"rawliteral(</div>
  
  <button class="btn arm-btn" onclick="location.href='/enable'">布防系统</button>
  <button class="btn disarm-btn" onclick="location.href='/disable'">撤防系统</button>
</body>
</html>
  )rawliteral";
  
  webServer.send(200, "text/html; charset=UTF-8", htmlPage);
}

// 布防功能处理
void enableSystem() {
  systemEnabled = true;
  alarmTriggered = false;
  digitalWrite(alarmLedPin, LOW);
  Serial.println("系统已成功布防");
  webServer.sendHeader("Location", "/");
  webServer.send(303);
}

// 撤防功能处理
void disableSystem() {
  systemEnabled = false;
  alarmTriggered = false;
  digitalWrite(alarmLedPin, LOW);
  Serial.println("系统已成功撤防");
  webServer.sendHeader("Location", "/");
  webServer.send(303);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(alarmLedPin, OUTPUT);
  digitalWrite(alarmLedPin, LOW);
  
  // WiFi 连接流程
  Serial.print("正在连接WiFi：");
  WiFi.begin(wifiName, wifiPwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接完成！");
  Serial.print("请访问：http://");
  Serial.println(WiFi.localIP());
  
  // 网页路由注册
  webServer.on("/", showHomePage);
  webServer.on("/enable", enableSystem);
  webServer.on("/disable", disableSystem);
  
  webServer.begin();
  Serial.println("Web服务已启动运行");
}

void loop() {
  webServer.handleClient();
  
  // 布防状态下检测触摸信号
  if (systemEnabled && !alarmTriggered) {
    int sensorVal = touchRead(touchSensorPin);
    Serial.printf("当前触摸数值：%d\n", sensorVal);
    
    if (sensorVal < touchTriggerLevel) {
      alarmTriggered = true;
      Serial.println("警告：触摸传感器已触发报警！");
    }
  }
  
  // 报警状态执行LED闪烁
  if (alarmTriggered) {
    unsigned long currentTime = millis();
    if (currentTime - previousFlashTime >= flashSpeed) {
      previousFlashTime = currentTime;
      digitalWrite(alarmLedPin, !digitalRead(alarmLedPin));
    }
  }
}