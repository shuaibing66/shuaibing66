// 作业5：触摸按键三档调节 呼吸灯亮度变化速度
// 功能说明：触摸GPIO4切换速度，LED实现慢/中/快 三档呼吸效果
#define TOUCH_SENSOR_PIN 4    // 触摸传感器引脚
#define PWM_LED_PIN 2         // PWM控制LED引脚

// LEDC PWM参数配置
const int pwmFrequency = 5000;
const int pwmBitDepth = 8;

// 触摸检测阈值 (触摸不灵敏可适当调大 700~800)
const int touchTriggerValue = 750;
int breathSpeedMode = 1;      // 1=慢速 2=中速 3=快速
bool previousTouchStatus = false;
unsigned long debounceTimer = 0;
const unsigned long debounceWait = 30;  // 缩短消抖时间，提升灵敏度

// 呼吸灯非阻塞变量
int pwmDuty = 0;
bool dirUp = true;   // 亮度方向：true变亮，false变暗
int controlDelay;
unsigned long breathTimer = 0;

void setup() {
  Serial.begin(115200);
  ledcAttach(PWM_LED_PIN, pwmFrequency, pwmBitDepth);
  Serial.println("触摸调速呼吸灯已启动");
}

void loop() {
  // ========== 1. 触摸检测 + 消抖（全程实时检测） ==========
  int sensorValue = touchRead(TOUCH_SENSOR_PIN);
  bool isTouched = (sensorValue < touchTriggerValue);

  // 标准消抖处理
  if (millis() - debounceTimer > debounceWait) {
    // 上升沿：按下瞬间切换档位
    if (isTouched && !previousTouchStatus) {
      breathSpeedMode++;
      if (breathSpeedMode > 3) {
        breathSpeedMode = 1;
      }
      Serial.print("当前呼吸速度档位：");
      Serial.println(breathSpeedMode);
    }
    previousTouchStatus = isTouched;
    debounceTimer = millis();
  }

  // ========== 2. 根据档位设置呼吸间隔 ==========
  switch (breathSpeedMode) {
    case 1: controlDelay = 20; break;
    case 2: controlDelay = 10; break;
    case 3: controlDelay = 5;  break;
    default: controlDelay = 20;
  }

  // ========== 3. 非阻塞呼吸灯（无delay，不阻塞触摸） ==========
  if (millis() - breathTimer >= controlDelay) {
    breathTimer = millis();

    if (dirUp) {
      pwmDuty++;
      if (pwmDuty >= 255) {
        dirUp = false;
      }
    } else {
      pwmDuty--;
      if (pwmDuty <= 0) {
        dirUp = true;
      }
    }
    ledcWrite(PWM_LED_PIN, pwmDuty);
  }
}