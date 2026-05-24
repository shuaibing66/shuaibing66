#include <WiFi.h>
#include <WebServer.h>

// -------------------------- 网络与硬件参数 --------------------------
const char* wifiName = "QAQ";
const char* wifiPass = "94pnnxzmr6gpifj";
const int touchInputPin = T0;  // 触摸传感器输入引脚 GPIO4
// -------------------------------------------------------------------

WebServer webServer(80);

// 首页：实时数据监控仪表盘
void showIndexPage() {
  String pageContent = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 触摸传感器监控面板</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 100px; }
    .panel { font-size: 120px; font-weight: bold; color: #20c997; }
    .tips { font-size: 24px; color: #555; margin-top: 20px; }
  </style>
</head>
<body>
  <h1>触摸信号实时监测</h1>
  <div class="panel" id="dataShow">0</div>
  <div class="tips">实时触摸数值（靠近时数值下降）</div>
  
  <script>
    const dataArea = document.getElementById('dataShow');

    // 定时刷新传感器数据
    setInterval(async () => {
      try {
        const res = await fetch('/readTouchData');
        const num = await res.text();
        dataArea.textContent = num;
      } catch (error) {
        console.error('数据获取异常:', error);
      }
    }, 100);
  </script>
</body>
</html>
  )rawliteral";
  
  webServer.send(200, "text/html; charset=UTF-8", pageContent);
}

// 读取并返回触摸传感器数值
void readSensorValue() {
  int currentTouchVal = touchRead(touchInputPin);
  webServer.send(200, "text/plain", String(currentTouchVal));
}

void setup() {
  Serial.begin(115200);
  
  // WiFi 连接过程
  Serial.print("正在连接无线网络...");
  WiFi.begin(wifiName, wifiPass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 连接成功！");
  Serial.print("控制面板地址: http://");
  Serial.println(WiFi.localIP());
  
  // 设置网页访问路径
  webServer.on("/", showIndexPage);
  webServer.on("/readTouchData", readSensorValue);
  
  webServer.begin();
  Serial.println("Web服务启动完成");
}

void loop() {
  webServer.handleClient();
}