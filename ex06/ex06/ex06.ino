// 作业6：双路PWM实现警车交替闪光效果
// 功能说明：使用ESP32的LEDC PWM功能，控制两个LED实现亮度互补、交替渐变闪烁

// 定义两个LED的控制引脚
#define MAIN_LED 4   // 主LED引脚，接GPIO4
#define EXT_LED 5    // 扩展LED引脚，接GPIO5

// 设定PWM运行参数
const int pwmHz = 5000;      // PWM频率设置为5000Hz（5千赫兹），保证灯光无频闪
const int pwmGrade = 8;      // PWM分辨率为8位，亮度范围0~255

void setup()
{
  Serial.begin(115200);      // 初始化串口波特率115200，用于调试输出

  // 为两路灯光分别配置PWM输出
  // ledcAttach(引脚, 频率, 分辨率)：将指定引脚绑定到LEDC PWM控制器
  ledcAttach(MAIN_LED, pwmHz, pwmGrade);  
  ledcAttach(EXT_LED, pwmHz, pwmGrade);
}

void loop()
{
  // 第一段：亮度逐步提升，两灯亮度互补切换
  // for循环：level从0慢慢增加到255，实现亮度从暗到亮的渐变
  for(int level = 0; level <= 255; level++)
  {
    // 主LED：亮度随level增大逐渐变亮
    ledcWrite(MAIN_LED, level);
    
    // 扩展LED：亮度随level增大逐渐变暗（255-level 与主灯互补）
    ledcWrite(EXT_LED, 255 - level);
    
    delay(10);  // 每步延时10毫秒，控制渐变速度
  }

  // 第二段：亮度逐步回落，保持交错闪烁状态
  // for循环：level从255慢慢减少到0，实现亮度从亮到暗的渐变
  for(int level = 255; level >= 0; level--)
  {
    // 主LED：亮度随level减小逐渐变暗
    ledcWrite(MAIN_LED, level);
    
    // 扩展LED：亮度随level减小逐渐变亮（继续保持互补）
    ledcWrite(EXT_LED, 255 - level);
    
    delay(10);  // 每步延时10毫秒，保持渐变速度平稳
  }
}