// 作业6：双路PWM模拟警车交替闪烁灯光效果
#define LIGHT_CH1 2
#define LIGHT_CH2 13

// PWM工作参数配置
const int pwmFreq = 5000;
const int pwmBitRange = 8;

void setup()
{
  Serial.begin(115200);
  // 分别绑定两路灯光输出通道
  ledcAttach(LIGHT_CH1, pwmFreq, pwmBitRange);
  ledcAttach(LIGHT_CH2, pwmFreq, pwmBitRange);
}

void loop()
{
  // 亮度递增，两灯亮度互补变化
  for(int brightness = 0; brightness <= 255; brightness++)
  {
    ledcWrite(LIGHT_CH1, brightness);
    ledcWrite(LIGHT_CH2, 255 - brightness);
    delay(10);
  }
  // 亮度递减，维持反向闪烁效果
  for(int brightness = 255; brightness >= 0; brightness--)
  {
    ledcWrite(LIGHT_CH1, brightness);
    ledcWrite(LIGHT_CH2, 255 - brightness);
    delay(10);
  }
}