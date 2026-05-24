// 作业5：触摸按键三档调节 呼吸灯亮度变化速度
// 功能说明：触摸GPIO4切换速度，LED实现慢/中/快 三档呼吸效果
#define TOUCH_SENSOR_PIN 4    // 触摸传感器引脚
#define PWM_LED_PIN 2         // PWM控制LED引脚

// LEDC PWM参数配置
const int pwmFrequency = 5000;
const int pwmBitDepth = 8;

// 触摸检测阈值
const int touchTriggerValue = 700;
int breathSpeedMode = 1;      // 1=慢速 2=中速 3=快速
bool previousTouchStatus = false;
unsigned long debounceTimer = 0;
const unsigned long debounceWait = 50;

void setup() {
  Serial.begin(115200);
  ledcAttach(PWM_LED_PIN, pwmFrequency, pwmBitDepth);  // PWM通道绑定
  Serial.println("触摸调速呼吸灯已启动");
}

void loop() {
  // 读取触摸传感器数值
  int sensorValue = touchRead(TOUCH_SENSOR_PIN);
  bool isTouched = (sensorValue < touchTriggerValue);

  // 触摸状态变化时重置消抖时间
  if (isTouched != previousTouchStatus) {
    debounceTimer = millis();
  }

  // 消抖完成后处理触摸事件
  if ((millis() - debounceTimer) > debounceWait) {
    // 检测到有效触摸时切换档位
    if (isTouched && !previousTouchStatus) {
      breathSpeedMode++;
      if (breathSpeedMode > 3) {
        breathSpeedMode = 1;  // 循环回到1档
      }
      // 串口打印当前档位
      Serial.print("当前呼吸速度档位：");
      Serial.println(breathSpeedMode);
    }
  }

  previousTouchStatus = isTouched;

  // 根据档位设置延时时间（控制呼吸快慢）
  int controlDelay;
  switch (breathSpeedMode) {
    case 1: controlDelay = 20; break;
    case 2: controlDelay = 10; break;
    case 3: controlDelay = 5;  break;
    default: controlDelay = 20;
  }

  // 呼吸灯：亮度逐渐变亮
  for (int pwmDuty = 0; pwmDuty <= 255; pwmDuty++) {
    ledcWrite(PWM_LED_PIN, pwmDuty);
    delay(controlDelay);
  }
  // 呼吸灯：亮度逐渐变暗
  for (int pwmDuty = 255; pwmDuty >= 0; pwmDuty--) {
    ledcWrite(PWM_LED_PIN, pwmDuty);
    delay(controlDelay);
  }
}