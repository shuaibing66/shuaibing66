// 作业8：借助millis函数模拟LED发出SOS求救灯光信号
// 信号规则：三下短亮200ms、三下长亮600ms、再三下短亮，间隙间隔200ms，整套信号间隔2000ms循环
const int ledIO = 2;

// 定义灯光运行状态分类
enum LightMode {
    FlashShortLight,
    FlashShortDark,
    FlashLongLight,
    FlashLongDark,
    SignalInterval
};
LightMode runMode = FlashShortLight;

int shortFlashNum = 0;
int longFlashNum = 0;
unsigned long recordStamp = 0;

// 各阶段定时时长参数
const int lightShortTime = 200;
const int gapShortTime = 200;
const int lightLongTime = 600;
const int gapLongTime = 200;
const int cycleWaitTime = 2000;

void setup() {
  pinMode(ledIO, OUTPUT);
  digitalWrite(ledIO, LOW);
  Serial.begin(115200);
  Serial.println("求救灯光信号已开启");
}

void loop() {
  unsigned long currTime = millis();

  switch (runMode)
  {
    case FlashShortLight:
      digitalWrite(ledIO, HIGH);
      if (currTime - recordStamp >= lightShortTime)
      {
        recordStamp = currTime;
        runMode = FlashShortDark;
        shortFlashNum++;
      }
      break;

    case FlashShortDark:
      digitalWrite(ledIO, LOW);
      if (currTime - recordStamp >= gapShortTime)
      {
        recordStamp = currTime;
        if (shortFlashNum < 3)
        {
          runMode = FlashShortLight;
        }
        else
        {
          longFlashNum = 0;
          runMode = FlashLongLight;
        }
      }
      break;

    case FlashLongLight:
      digitalWrite(ledIO, HIGH);
      if (currTime - recordStamp >= lightLongTime)
      {
        recordStamp = currTime;
        runMode = FlashLongDark;
        longFlashNum++;
      }
      break;

    case FlashLongDark:
      digitalWrite(ledIO, LOW);
      if (currTime - recordStamp >= gapLongTime)
      {
        recordStamp = currTime;
        if (longFlashNum < 3)
        {
          runMode = FlashLongLight;
        }
        else
        {
          shortFlashNum = 0;
          runMode = FlashShortLight;
        }
      }
      break;

    case SignalInterval:
      digitalWrite(ledIO, LOW);
      if (currTime - recordStamp >= cycleWaitTime)
      {
        recordStamp = currTime;
        shortFlashNum = 0;
        longFlashNum = 0;
        runMode = FlashShortLight;
        Serial.println("单次SOS信号周期结束");
      }
      break;
  }

  // 整套信号结束后进入待机间隔
  if(shortFlashNum == 3 && longFlashNum == 3 && runMode == FlashShortDark)
  {
    runMode = SignalInterval;
    recordStamp = currTime;
  }
}
