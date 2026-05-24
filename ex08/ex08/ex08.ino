#include <WiFi.h>
#include <WebServer.h>

// WiFi参数，和你之前保持一致
const char* wifiName = "QAQ";
const char* wifiPwd = "94pnnxzmr6gpifj";

// 硬件定义
const int ledPin = 2;
const int touchPin = T0;

WebServer server(80);
// 系统状态
bool systemArmed = false;
bool alarmTriggered = false;

// 网页控制界面
void showWebUI()
{
  String htmlPage = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>安防报警器</title>
<style>
body { text-align: center; margin-top: 90px; }
button { font-size: 22px; padding: 12px 35px; margin: 8px; }
#arm { background: #28a745; color: white; border: none; }
#disarm { background: #dc3545; color: white; border: none; }
</style>
</head>
<body>
<h2>ESP32 安防报警系统</h2>
<button id="arm" onclick="sendOrder('arm')">布防 Arm</button>
<button id="disarm" onclick="sendOrder('disarm')">撤防 Disarm</button>

<script>
function sendOrder(cmd) {
  fetch("/" + cmd);
}
</script>
</body>
</html>
)HTML";
  server.send(200, "text/html; charset=utf-8", htmlPage);
}

// 布防功能
void enableArm()
{
  systemArmed = true;
  server.send(200, "text/plain", "已进入布防模式");
}

// 撤防并重置报警
void disableArm()
{
  systemArmed = false;
  alarmTriggered = false;
  digitalWrite(ledPin, LOW);
  server.send(200, "text/plain", "已撤防，系统复位");
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // 连接WiFi
  WiFi.begin(wifiName, wifiPwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(350);
  }
  Serial.print("设备IP：");
  Serial.println(WiFi.localIP());

  // 配置网页路由
  server.on("/", showWebUI);
  server.on("/arm", enableArm);
  server.on("/disarm", disableArm);
  server.begin();
}

void loop()
{
  server.handleClient();

  // 布防状态下检测触摸触发报警
  if (systemArmed && !alarmTriggered)
  {
    if (touchRead(touchPin) < 45)
    {
      alarmTriggered = true;
    }
  }

  // 报警后LED持续高频闪烁，直到撤防
  if (alarmTriggered)
  {
    digitalWrite(ledPin, HIGH);
    delay(75);
    digitalWrite(ledPin, LOW);
    delay(75);
  }
}