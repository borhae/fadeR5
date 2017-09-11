#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

int poti = 35;// pin D35

// GUItool: begin automatically generated code
//AudioInputI2S            i2s1;           //xy=200,69
AudioInputUSB            i2s1;           //xy=200,69
AudioOutputI2S           i2s2;           //xy=365,94
AudioConnection          patchCord1(i2s1, 0, i2s2, 0);
AudioConnection          patchCord2(i2s1, 1, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=302,184
// GUItool: end automatically generated code
const int myInput = AUDIO_INPUT_LINEIN;
elapsedMillis volUpdateTimer = 0;

void setup() 
{
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
  int wiperVal = analogRead(poti);
  if(volUpdateTimer > 50)
  {
    volUpdateTimer= 0;
    float vol = mapFloat(wiperVal, 0, 1024, 0.0, 1.0);
    Serial.print("Volume ");
    Serial.println(vol);
    sgtl5000_1.volume(vol);
  }
}

float mapFloat(int val, int inLow, int inHigh, float outLow, float outHigh)
{
  float result = outLow;
  result = (float) val / ((float) inHigh - (float) inLow); // normalize
  result = result * (outHigh - outLow);
  return result;
}

