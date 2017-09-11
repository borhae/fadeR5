#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

int motorA = 36; // pin D36, PWM
int motorB = 37; // pin D37, PWM
int motorEnable = 38;// pin D38

int poti = 35;// pin D35
int touch = 30; //pin D30
int targets[] = {800, 100, 1000, 0, 500};
int curTargetIdx = 0;
int faderThreshold = 20;

float wiperIn = 0.0;
// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=200,69
AudioOutputI2S           i2s2;           //xy=365,94
AudioConnection          patchCord1(i2s1, 0, i2s2, 0);
AudioConnection          patchCord2(i2s1, 1, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=302,184
// GUItool: end automatically generated code
const int myInput = AUDIO_INPUT_LINEIN;



void setup() 
{
  pinMode(motorEnable, OUTPUT);
  pinMode(touch, INPUT);
  Serial.begin(9600);
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);
}

void loop() 
{
  int currentTarget = 0;
  boolean targetReached = false;
  boolean moveUp = false;
  boolean moveDown = false;
  if(touchRead(touch) > 1800)
  {
    return;
  }
  while(!targetReached)
  {
    int touchVal = touchRead(touch);
    wiperIn = analogRead(poti);
    digitalWrite(motorEnable, HIGH);
    currentTarget = targets[curTargetIdx];
    if(wiperIn < (currentTarget - faderThreshold))
    {
      if(touchRead(touch) > 1800)
      {
        return;
      }
      targetReached = false;
      moveUp = true;
      moveDown = false;
      analogWrite(motorA, 255);
      analogWrite(motorB, 0);
    }
    else if(wiperIn > (currentTarget + faderThreshold))
    {
      targetReached = false;
      moveUp = false;
      moveDown = true;
      analogWrite(motorA, 0);
      analogWrite(motorB, 255);
    }
    else
    {
      digitalWrite(motorEnable, LOW);
      targetReached = true;
      moveUp = false;
      moveDown = false;
      printOutData(currentTarget, moveUp, moveDown, wiperIn, touchVal); 
    }
    if(millis() % 100 == 0)
     {
      Serial.print("currentTarget ");
      Serial.print(currentTarget);
      Serial.print(" ,moveUp ");
      Serial.print(moveUp);
      Serial.print(" ,moveDown ");
      Serial.print(moveDown);
      Serial.print(" , wiperIn ");
      Serial.print(wiperIn);
      Serial.print(" touch ");
      Serial.print(touchVal);
      Serial.print(" time ");
      Serial.println(millis());
    }
  }
  if(curTargetIdx < (sizeof(targets) - 1))
  {
    curTargetIdx++;
  }
  else
  {
    curTargetIdx = 0;
  }
  for(int waitCnt = 0; waitCnt < 100; waitCnt++)
  {
      Serial.print("currentTarget ");
      Serial.print(currentTarget);
      Serial.print(" ,moveUp ");
      Serial.print(moveUp);
      Serial.print(" ,moveDown ");
      Serial.print(moveDown);
      Serial.print(" , wiperIn ");
      Serial.print(wiperIn);
      int touchVal = touchRead(touch);
      Serial.print(" touch ");
      Serial.print(touchVal);
      Serial.print(" time ");
      Serial.println(millis());
      delay(20);
  }
}

  void printOutData(int currentTarget, boolean moveUp, boolean moveDown, float wiperIn, int touchVal)
  {
      Serial.print("currentTarget ");
      Serial.print(currentTarget);
      Serial.print(" ,moveUp ");
      Serial.print(moveUp);
      Serial.print(" ,moveDown ");
      Serial.print(moveDown);
      Serial.print(" , wiperIn ");
      Serial.print(wiperIn);
      Serial.print(" time ");
      Serial.println(millis());
  }

