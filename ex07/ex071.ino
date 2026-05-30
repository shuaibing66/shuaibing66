#include <WiFi.h>
#include <WebServer.h>

// ########### 请修改为你自己的WiFi名称和密码 ###########
const char* ssid     = "庞庞帅兵的iPhone";
const char* password = "12345678";

#define LED_PIN 2        // LED引脚，避开启动引脚，可改用4/5
const int pwmFreq = 5000;
const int pwmBit = 8;

WebServer server(80);    // 网页服务器，端口80
int ledDuty = 0;         // PWM亮度值 0~255

// 网页HTML内容（包含滑动条 + JS监听）
const char* htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 网页无极调光</title>
    <style>
        body {text-align:center;margin-top:50px;font-size:20px;}
        .slider {width:80%;height:30px;margin:30px 0;}
    </style>
</head>
<body>
    <h2>LED 亮度调节</h2>
    <p>当前亮度值：<span id="val">0</span></p>
    <!-- 滑动条：范围 0~255 -->
    <input type="range" class="slider" id="bright" min="0" max="255" value="0">

    <script>
        const slider = document.getElementById("bright");
        const showVal = document.getElementById("val");

        // 监听滑动条拖动事件
        slider.addEventListener("input", function(){
            let num = this.value;
            showVal.innerText = num;
            // 发送GET请求到ESP32，传递亮度值
            fetch("/set?duty=" + num);
        });
    </script>
</body>
</html>
)HTML";

// 根路径：返回调光网页
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// 接收亮度参数，控制PWM
void handleSetDuty() {
  if (server.hasArg("duty")) {
    ledDuty = server.arg("duty").toInt();
    ledcWrite(LED_PIN, ledDuty);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  // 初始化PWM
  ledcAttach(LED_PIN, pwmFreq, pwmBit);

  // 连接WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi已连接，IP地址：");
  Serial.println(WiFi.localIP());

  // 注册网页路由
  server.on("/", handleRoot);
  server.on("/set", handleSetDuty);

  server.begin();
  Serial.println("Web服务器启动成功");
}

void loop() {
  server.handleClient(); // 持续处理网页请求
}