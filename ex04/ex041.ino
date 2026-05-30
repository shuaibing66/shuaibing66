#define TOUCH_PIN 4
#define LED_PIN 2

// 触摸与防抖参数
const int TOUCH_THRESHOLD = 700;
const unsigned long DEBOUNCE_TIME = 30;

// 状态变量
bool ledState = false;        // LED自锁状态
bool lastTouchState = false;  // 上一次触摸状态
unsigned long debounceTime = 0;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(115200);
  Serial.println("触摸自锁开关就绪");
}

void loop()
{
  // 读取触摸值
  int touchVal = touchRead(TOUCH_PIN);
  bool nowTouch = (touchVal < TOUCH_THRESHOLD);

  // 软件防抖
  if (millis() - debounceTime > DEBOUNCE_TIME)
  {
    // 边缘检测：仅 未触摸 → 触摸 的瞬间翻转状态
    if (nowTouch == true && lastTouchState == false)
    {
      ledState = !ledState;       // 状态翻转，实现自锁
      digitalWrite(LED_PIN, ledState);

      if(ledState)
      {
        Serial.println("LED 点亮");
      }
      else
      {
        Serial.println("LED 熄灭");
      }
    }
    // 更新历史状态
    lastTouchState = nowTouch;
    debounceTime = millis();
  }
}