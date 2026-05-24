// 作业6：双路PWM实现警车交替闪光效果
#define MAIN_LED 2
#define EXT_LED 13

// 设定PWM运行参数
const int pwmHz = 5000;
const int pwmGrade = 8;

void setup()
{
  Serial.begin(115200);
  // 为两路灯光分别配置PWM输出
  ledcAttach(MAIN_LED, pwmHz, pwmGrade);
  ledcAttach(EXT_LED, pwmHz, pwmGrade);
}

void loop()
{
  // 亮度逐步提升，两灯亮度互补切换
  for(int level = 0; level <= 255; level++)
  {
    ledcWrite(MAIN_LED, level);
    ledcWrite(EXT_LED, 255 - level);
    delay(10);
  }
  // 亮度逐步回落，保持交错闪烁状态
  for(int level = 255; level >= 0; level--)
  {
    ledcWrite(MAIN_LED, level);
    ledcWrite(EXT_LED, 255 - level);
    delay(10);
  }
}