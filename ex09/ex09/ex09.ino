#include <WiFi.h>
#include <WebServer.h>

// ========== 配置项 ==========
const char* ssid     = "庞庞帅兵的iPhone";
const char* password = "12345678";
#define TOUCH_PIN 4      // 触摸传感器引脚 GPIO4
WebServer server(80);

// 网页 HTML + AJAX 代码
const char* htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>触摸传感器实时仪表盘</title>
    <style>
        body{text-align:center;margin-top:80px;font-family:微软雅黑;}
        .title{font-size:28px;color:#333;}
        .data-box{
            font-size:72px;
            font-weight:bold;
            color:#0066cc;
            margin:40px 0;
        }
        .tip{font-size:18px;color:#666;}
    </style>
</head>
<body>
    <div class="title">触摸传感器实时数值</div>
    <div class="data-box" id="touchVal">0</div>
    <div class="tip">手指靠近引脚 → 数值变小 | 手指离开 → 数值变大</div>

    <script>
        // 定时AJAX轮询，100ms拉取一次数据
        function getData(){
            fetch("/getTouch")
                .then(res => res.text())
                .then(val => {
                    document.getElementById("touchVal").innerText = val;
                });
        }
        // 循环执行，实现实时刷新
        setInterval(getData, 100);
    </script>
</body>
</html>
)HTML";

// 路由1：返回主网页
void handleRoot()
{
  server.send(200, "text/html", htmlPage);
}

// 路由2：读取触摸值并返回给网页
void handleGetTouch()
{
  int val = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(val));
}

void setup()
{
  Serial.begin(115200);

  // 连接WiFi
  WiFi.begin(ssid, password);
  Serial.print("WiFi连接中...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(400);
    Serial.print(".");
  }
  Serial.println("\n连接成功！");
  Serial.print("设备IP：");
  Serial.println(WiFi.localIP());

  // 注册网页路由
  server.on("/", handleRoot);
  server.on("/getTouch", handleGetTouch);

  server.begin();
  Serial.println("Web仪表盘服务已启动");
}

void loop()
{
  server.handleClient(); // 持续处理网页请求
}