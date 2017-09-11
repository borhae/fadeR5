#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <math.h>

int motorAPin = 36; // pin D36, PWM
int motorBPin = 37; // pin D37, PWM
int motorEnablePin = 38;// pin D38, PWM

int fader1Pin = 35;// pin D35
int touch1Pin = 30; //pin D30
int poti1Pin = 31; // pin D31

// GUItool: begin automatically generated code
//AudioInputI2S            i2s1;           //xy=462,240
AudioInputUSB          i2s1;           //xy=200,69
AudioAnalyzePeak         peak1;          //xy=583,131
AudioAnalyzePeak         peak2;          //xy=604,315
AudioOutputI2S           i2s2;           //xy=768,253
AudioConnection          patchCord1(i2s1, 0, i2s2, 0);
AudioConnection          patchCord2(i2s1, 0, peak1, 0);
AudioConnection          patchCord3(i2s1, 1, i2s2, 1);
AudioConnection          patchCord4(i2s1, 1, peak2, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=483,447
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;
elapsedMillis trickleDownTimer = 0;

int curTarget = 512;

int fader1Val = 0;
int touch1Val = 0;
int poti1Val = 0;
float curPeak = 0.0;
int curTrickle = 5;
int fader1Speed = 255;
boolean fader1PeakReached = false;
int tolerance = 20;

String faderDirection = "";
String UP = "up";
String DOWN = "down";
String STOP = "stop";

void setup() 
{
  Serial.begin(9600);
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  pinMode(touch1Pin, INPUT);
  pinMode(motorEnablePin, OUTPUT);
  
  AudioMemory(12);
  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.8);
}

void loop() 
{
  fader1Val = analogRead(fader1Pin);
  touch1Val = touchRead(touch1Pin);
  poti1Val = analogRead(poti1Pin);
  moveFader(curTarget, fader1Speed, fader1Val);
  if(trickleDownTimer > 25)
  {
    if(curTrickle > 10)
    {
      curTrickle = curTrickle - 5;
    }
    trickleDownTimer = 0;
  }
  if(peak1.available() && peak2.available())
  {
    float peakLeft = peak1.read();
    float peakRight = peak2.read();
    curPeak = ((peakLeft + peakRight) * poti1Val * 10);
    if(curPeak > 1020)
    {
      curPeak = 1020;
    }
  }  
  if(curTrickle < curPeak)
  {
    curTarget = curPeak;
    curTrickle = curPeak;
    fader1Speed = 255;
    fader1PeakReached = false;
  } 
  else
  {
    fader1PeakReached = fader1Val >= curTarget - tolerance;
    if(fader1PeakReached)
    {
      curTarget = 30;
      fader1Speed = 150;
    }
  }
  Serial.print("speed ");
  Serial.print(fader1Speed);
  Serial.print(" target ");
  Serial.print(curTarget);
  Serial.print(" fader pos ");
  Serial.print(fader1Val);
  Serial.print(" direction Change value ");
  Serial.print((fader1Val - tolerance));
  Serial.print(" direction ");
  Serial.print(faderDirection);
  Serial.print(" curTrickle ");
  Serial.println(curTrickle);
}

void moveFader(int target, int moveSpeed, int curPos)
{
  if(curPos < (target - tolerance))
  {
    moveUp(moveSpeed);
    faderDirection = UP;
  }
  else if(curPos > (target + tolerance))
  {
    moveDown(moveSpeed);
    faderDirection = DOWN;
  }
  else
  {
    analogWrite(motorEnablePin, 0);
    faderDirection = STOP;
  }
}

void moveUp(int moveSpeed)
{
  analogWrite(motorEnablePin, moveSpeed);
  analogWrite(motorAPin, 255);
  analogWrite(motorBPin, 0);
}

void moveDown(int moveSpeed)
{
  analogWrite(motorEnablePin, moveSpeed);
  analogWrite(motorAPin, 0);
  analogWrite(motorBPin, 255);
}

float mapFloat(int val, int inLow, int inHigh, float outLow, float outHigh)
{
  float result = outLow;
  result = ((float) val / ((float) inHigh - (float) inLow)) - ((float) inLow); // normalize
  result = result * (outHigh - outLow); // scale
  result = result + outLow; // offset
  return result;
}

int mapIntExp(int val, int inLow, int inHigh, int outLow, int outHigh)
{
  float result = 0;
  float normalizedVal = 0;
  normalizedVal = ((float) val / ((float) inHigh - (float) inLow)) - ((float) inLow);
  float factor = log((float)outHigh);
  result = exp(normalizedVal * factor);
  return (int)result;
}

