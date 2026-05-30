#include <WiFi.h>
#include <WebServer.h>

// WiFi名称和密码，用于ESP32连接手机热点
const char* wifiName = "庞庞帅兵的iPhone";
const char* wifiPwd = "12345678";

// 硬件引脚定义
// LED连接在GPIO2引脚，用于报警时闪烁提示
const int ledPin = 2;
// 触摸传感器使用ESP32内置触摸通道T0（对应GPIO4）
const int touchPin = T0;

// 创建Web服务器对象，使用80端口（网页默认端口）
WebServer server(80);

// 系统状态变量
// systemArmed：标记系统是否处于布防状态
bool systemArmed = false;
// alarmTriggered：标记是否触发了报警
bool alarmTriggered = false;

// 网页控制界面函数
// 功能：当用户访问ESP32的IP地址时，显示布防/撤防按钮页面
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
  // 将网页内容发送给浏览器
  server.send(200, "text/html; charset=utf-8", htmlPage);
}

// 布防功能函数
// 功能：设置系统为布防状态，准备检测触摸
void enableArm()
{
  systemArmed = true;  // 将布防状态标记为true
  server.send(200, "text/plain", "已进入布防模式");  // 网页返回提示信息
}

// 撤防功能函数
// 功能：关闭布防、关闭报警、熄灭LED，系统恢复待机
void disableArm()
{
  systemArmed = false;     // 取消布防
  alarmTriggered = false;  // 取消报警
  digitalWrite(ledPin, LOW); // 熄灭LED
  server.send(200, "text/plain", "已撤防，系统复位"); // 网页返回提示
}

void setup()
{
  // 初始化串口波特率115200，用于电脑查看调试信息
  Serial.begin(115200);
  
  // 设置LED引脚为输出模式
  pinMode(ledPin, OUTPUT);
  // 初始状态LED熄灭
  digitalWrite(ledPin, LOW);

  // 开始连接WiFi
  WiFi.begin(wifiName, wifiPwd);
  // 等待WiFi连接成功，期间串口打印提示
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(350);
    Serial.print("正在连接WiFi...");
  }
  
  // WiFi连接成功后，串口打印ESP32的IP地址
  Serial.println("WiFi 连接成功！");
  Serial.print("ESP32 设备IP：");
  Serial.println(WiFi.localIP());

  // 设置网页路由：访问根目录 / 时显示控制界面
  server.on("/", showWebUI);
  // 访问 /arm 路径时执行布防
  server.on("/arm", enableArm);
  // 访问 /disarm 路径时执行撤防
  server.on("/disarm", disableArm);
  
  // 启动Web服务器
  server.begin();
  Serial.println("Web服务器已启动！");
}

void loop()
{
  // 处理网页客户端的请求（必须不断调用）
  server.handleClient();

  // ===================== 新增功能：实时打印触摸传感器数值 =====================
  // 功能：读取触摸引脚的当前数值，并通过串口打印出来
  // 作用：方便调试、观察触摸灵敏度，方便老师查看触摸状态
  int touchValue = touchRead(touchPin);  // 读取触摸值并保存到变量中
  Serial.print("当前触摸传感器数值：");   // 打印提示文字
  Serial.println(touchValue);            // 打印真实的触摸数值
  // ==========================================================================

  // 判断逻辑：如果系统已布防，并且还没有触发报警
  if (systemArmed && !alarmTriggered)
  {
    // 触摸值 < 45 表示有人触摸传感器（数值越小，触摸越明显）
    if (touchRead(touchPin) < 45)
    {
      Serial.println("!!! 检测到触摸，报警已触发 !!!"); // 新增：报警时串口提示
      alarmTriggered = true; // 将报警状态标记为true
    }
  }

  // 如果报警状态为true，则LED快速闪烁
  if (alarmTriggered)
  {
    digitalWrite(ledPin, HIGH); // LED亮
    delay(75);                  // 保持亮75毫秒
    digitalWrite(ledPin, LOW);  // LED灭
    delay(75);                  // 保持灭75毫秒
  }
}