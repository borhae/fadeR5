#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <math.h>

int fader1Pin = 35;// pin D35
int touch1Pin = 30; //pin D30
int poti1Pin = 31; // pin D31

// GUItool: begin automatically generated code
//AudioInputI2S            i2s1;           //xy=200,69
AudioInputUSB            i2s1;           //xy=200,69
AudioOutputI2S           i2s2;           //xy=365,94
AudioConnection          patchCord1(i2s1, 0, i2s2, 0);
AudioConnection          patchCord2(i2s1, 1, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=302,184
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_LINEIN;
elapsedMillis updateTimer = 0;

int filterSettings[5];

void setup() 
{
  Serial.begin(9600);
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  pinMode(touch1Pin, INPUT);
  
  AudioMemory(12);
  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.8);
  sgtl5000_1.audioPostProcessorEnable(); // enable the DAP block in SGTL5000
//calcBiquad(FilterType,FrequencyC,dBgain (from -7 to 7),Q,QuantizationUnit,SampleRate,int*);
  calcBiquad(FILTER_PARAEQ, 110, 0, 0.2, 524288, 44100, filterSettings);
//eqFilter(FilterNumber, Coefficients[5]);
  sgtl5000_1.eqFilter(0, filterSettings);
}

void loop() 
{
  int fader1Val = analogRead(fader1Pin);
  int touch1Val = touchRead(touch1Pin);
  int poti1Val = analogRead(poti1Pin);
  if(updateTimer > 50)
  {
    updateTimer = 0;
    float filterGain = mapFloat(fader1Val, 0, 1024, -7.0, 7.0);
    int cutoffFreq = mapIntExp(poti1Val, 0, 1024, 0, 22000);
//calcBiquad(FilterType,FrequencyC,dBgain (from -7 to 7),Q,QuantizationUnit,SampleRate,int*);
    calcBiquad(FILTER_PARAEQ, cutoffFreq, filterGain, 0.2, 524288, 44100, filterSettings);
//eqFilter(FilterNumber, Coefficients[5]);
    sgtl5000_1.eqFilter(0, filterSettings);
    printData(filterGain, cutoffFreq);
  }
}

void printData(float filterGain, int cutoffFreq)
{
  Serial.print("gain ");
  Serial.print(filterGain);
  Serial.print(" cutoff frequency ");
  Serial.print(cutoffFreq);
  Serial.println("");
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

