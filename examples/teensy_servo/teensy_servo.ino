#include <SD.h>
#include <SPI.h>
#include <TeensyAnima.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

extern AudioPlaySdWav           playWav1;
extern AudioControlSGTL5000     sgtl5000_1;

// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
//AudioOutputI2S           audioOutput;
//AudioOutputSPDIF       audioOutput;
//AudioOutputAnalog      audioOutput;
//AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
//AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
//AudioControlSGTL5000     sgtl5000_1;

// Use these with the Teensy Audio Shield
//#define SDCARD_CS_PIN    10
//#define SDCARD_MOSI_PIN  7
//#define SDCARD_SCK_PIN   14

// Use these with the Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

// Use these for the SD+Wiz820 or other adaptors
//#define SDCARD_CS_PIN    4
//#define SDCARD_MOSI_PIN  11
//#define SDCARD_SCK_PIN   13

const int chipSelect = BUILTIN_SDCARD;

int dancer;

//#include "core_cm4.h"
extern char *__brkval;
extern unsigned long _ebss;
extern unsigned long _estack;

void setup() {
      Serial.print((int)(__brkval),HEX);
      Serial.print("  ");
      Serial.print((int)&_estack,HEX);
      Serial.println();
  pinMode(13,OUTPUT);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  AudioMemory(8);

  Serial.println("Hello");

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");

      Serial.print((int)(__brkval),HEX);
      Serial.print("  ");
      Serial.print((int)&_estack,HEX);
      Serial.println();

  openAnimaKernel();
  dancer = openDancer(-1,0);
  setDancerFile(dancer, (char *)"PETRUSJ.MOV", NULL);
  loadDancerFile(dancer);

  doStart();
  //playWav1.play("MECATRON.WAV");
}

uint32_t frameTime = 0;
uint32_t frameTime_uS = 0;
uint32_t fcount = 0;
int playingaudio=1;

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, millis()%1000<100);

  if (millis()-frameTime>=10) {
    frameTime = millis();      
    frameTime_uS = micros();      
    kernelExec(0);
    fcount++;
    if (fcount==100) {
      Serial.print(micros()-frameTime_uS);
      Serial.print("  ");
      Serial.println(GetClockInfo(CLK_TIME));
      Serial.print((int)__brkval,HEX);
      Serial.print("  ");
      Serial.print((int)&_estack,HEX);
      Serial.print("  ");
//      Serial.print(__get_MSP(),HEX);
      Serial.println();
      fcount = 0;
    }
    if (playingaudio && !playWav1.isPlaying()) {
      Serial.println("Sound finished");
      playingaudio = 0;
    }
  }
  while (Serial.available()) {
    char c = Serial.read();
    if (c=='s') doStart();
    if (c=='x') doStop();
    if (c=='r') doRewind();
  }

}
