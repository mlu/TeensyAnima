/****************************************************
 * 
 * Anima motion control for the Petrusjka robotic doll
 * - Running on Teensy 3.6 board
 * - Teensy SGTL5000 Audio Adapter
 * - Pololu servo interface
 * 
 * 
 * Magnus Lundin 2018
 * 
 ****************************************************/

#include <TeensyAnima.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

/*******************************************************
 * 
 * Pin Configuration
 * 
 *******************************************************/

/* Use Serial4 on pin32 for servo controller output
 * file: libraries/TeensyAnima/pololuservo.cpp:26
 * HardwareSerial * Port = &Serial4;
 */

#define Nextion Serial5

#define HALL_PULSE_PIN 38

// Use Teensy 3.6 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

/*******************************************************
 * 
 * Globals
 * 
 *******************************************************/

extern float gPulseActivity;

int dancer;

/*******************************************************
 * 
 * Audio interface
 * 
 *******************************************************/

extern AudioPlaySdWav           playWav1;
extern AudioControlSGTL5000     sgtl5000_1;

// Audio implementation is found in TeensyAnima/tsoundproc.cpp

const int chipSelect = BUILTIN_SDCARD;

/*******************************************************
 * 
 * Nextion HMI user interface
 * 
 *******************************************************/
/*  Name ,   cid,   function
 *  
 *  "b0"       2      Start
 *  "b1"       3      Stop
 *  
 *  "bt0"      9      Set Auto/Pulsecontrol mode
 *  "bt1"     10      Set Manual/No pulsecontrol mode
 *  
 *  "t1"              Status display
 *  "t2"              Time display text string
 *
 *  "j0"              Activity bar
 *  
 */
#define NEXTION_STATE_IDLE  0
#define NEXTION_STATE_TOUCHEVT  1
#define NEXTION_STATE_NUMERIC  2
#define NEXTION_STATE_STRING   3

int NextionState = NEXTION_STATE_IDLE;
int endcount = 0;
int nextionpos = 0;

/* Remember event status */
int NextionPageId = 0;
int NextionCompId = 0;
int NextionEvent = 0;
uint32_t NextionValue = 0;

void NextionHandleEvent(uint8_t pid, uint8_t cid, uint8_t evnt) {
/* This is where we actually handle user interface events */
  Serial.printf("Event page:%i, component:%i, event:%i \n",pid,cid,evnt);
/* This is where we actually handle user interface events */
  if (evnt == 0) {
    NextionCompId = -1;
  }
  else {
    if (cid == 2) {doStart();};
    if (cid == 3) {doStop();};
    /* Manual */
    if (cid == 10) setDancerOption(dancer, "PULSECONTROL", 0);
    /* Auto */
    if (cid == 9) setDancerOption(dancer, "PULSECONTROL", 1);
  };
}

void NextionHandleValue(uint32_t value) {
/* This is where we actually handle user interface events */

}

void NextionShowTime() {
  int t = GetClockInfo(CLK_TIME);
  char text[64];
  sprintf(text,"%i:%i.%i",t/6000,(t/100)%60,t%100);
  Serial.printf("%s\n",text);
  NextionSetText("t2",text);
}

void NextionShowActivity() {
  int activity = 50*gPulseActivity;
  if (activity>100) activity = 100;
  if (activity<26) Nextion.printf("j0.pco=63488\xFF\xFF\xFF");
  else if (activity<50) Nextion.printf("j0.pco=64512\xFF\xFF\xFF");
  else if (activity<70) Nextion.printf("j0.pco=65504\xFF\xFF\xFF");
  else Nextion.printf("j0.pco=1024\xFF\xFF\xFF");
  NextionSetValue("j0",activity);
}

void NextionParseEvent(uint8_t c) {
  if (c == 255) endcount++;
  else endcount = 0;

  if (endcount == 3) {
    endcount = 0;
    NextionState = NEXTION_STATE_IDLE;
  }

  switch (NextionState) {
    case NEXTION_STATE_IDLE: {
      nextionpos = 0;
      if (c==0x65) NextionState = NEXTION_STATE_TOUCHEVT;
      if (c==0x70) NextionState = NEXTION_STATE_STRING;
      if (c==0x71) NextionState = NEXTION_STATE_NUMERIC;
      break;
    }

    case NEXTION_STATE_TOUCHEVT: {
      if (nextionpos == 0) {
        NextionPageId = c;
      }
      if (nextionpos == 1) {
        NextionCompId = c;
      }
      if (nextionpos == 2) {
        NextionEvent = c;
        /* Call callback NextionHandleEvent() */
        NextionHandleEvent(NextionPageId, NextionCompId, NextionEvent);
      }
      nextionpos++;
      break;
    }

    case NEXTION_STATE_NUMERIC: {
      if (nextionpos == 0) {
        NextionValue = c;
      }
      if (nextionpos == 1) {
        NextionValue |= (c<<8);
      }
      if (nextionpos == 2) {
        NextionValue |= (c<<16);
      }
      if (nextionpos == 3) {
        NextionValue |= (c<<24);
        /* Call callback NextionHandleEvent() */
        NextionHandleValue(NextionValue);
      }
      nextionpos++;
      break;
    }

  }
}

void NextionSetText(char * objname,char * txt) {
  Nextion.printf("%s.txt=\"%s\"\xFF\xFF\xFF",objname,txt);
}

void NextionSetValue(char * objname,int val) {
  Nextion.printf("%s.val=%i\xFF\xFF\xFF",objname,val);
}

/*******************************************************
 * 
 * Heap testing
 * 
 *******************************************************/

/* Test to see if there are stack or heap issues */
extern char *__brkval;
extern unsigned long _ebss;
extern unsigned long _estack;

void checkHeap() {

      Serial.println(GetClockInfo(CLK_TIME));
      Serial.print((int)__brkval,HEX);
      Serial.print("  ");
      Serial.print((int)&_estack,HEX);
      Serial.print("  ");
//      Serial.print(__get_MSP(),HEX);

}

/*******************************************************
 * 
 * Debounce and pulse counting
 * 
 *******************************************************/

#define DEBOUNCE_COUNT 3

uint32_t debounce_sequence[DEBOUNCE_COUNT];
uint32_t debounce_ix = 0;
uint32_t debounce_value;
uint32_t debounce_change;
uint32_t debounce_rise;
uint32_t debounce_fall;
uint32_t encoder[8];

void debounce(uint32_t newbits) {
    uint32_t mark=0xFFFFFFFF;
    uint32_t space=0;
    debounce_ix = (debounce_ix+1)%DEBOUNCE_COUNT;
    debounce_sequence[debounce_ix] = newbits;
    for (int k=0;k<DEBOUNCE_COUNT;k++) {
        mark = mark&debounce_sequence[k];
        space = space|debounce_sequence[k];
    }
    debounce_rise = (~debounce_value)&mark;
    debounce_fall = debounce_value&~space;
    debounce_change = debounce_rise|debounce_fall;
    debounce_value = debounce_value^debounce_change;
    // If rise, fall and change are not needed we can use
    // debounce_value = (debounce_value|mark)&space;
}

/*  PulseCount is called from 
 *  file: libraries/TeensyAnima/mpetrusjka.cpp:26
 */

uint32_t pulse_count = 0;
uint32_t pulse_state = 0;

uint32_t PulseCount() {  
  /* Hall sensor pulse counter for Teensy 3.6 */
  uint32_t ns=digitalRead(HALL_PULSE_PIN);
  if (debounce_rise) {
    pulse_count++;
    pulse_state=ns;
  }
  return pulse_count;
}

/*******************************************************
 * 
 * Main setup and loop
 * 
 *******************************************************/
 
void setup() {

  pinMode(13,OUTPUT);
  pinMode(HALL_PULSE_PIN, INPUT_PULLUP);

  /* Audio board control */
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  AudioMemory(8);

  /* Nextion HMI board */
//  Nextion.begin(115200);
//  Nextion.begin(57600);
//  Nextion.begin(9600);
  Nextion.begin(19200);
  delay(100);
  Nextion.printf("\xFF\xFF\xFF");
  delay(100);
  Nextion.printf("dim=40\xFF\xFF\xFF");

  Serial.println("Hello");

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");

  openAnimaKernel();
  dancer = openDancer(-1, DANCER_PETRUSJKA);
  setDancerFile(dancer, (char *)"PETRUSJ.MOV", NULL);
  Serial.println(dancer);

  int res = loadDancerFile(dancer);
  if (res>0) {
    Serial.println("Choreography Loaded");
    setDancerOption(dancer, "PULSECONTROL", 1);
    NextionSetValue("bt0", 1);
    NextionSetValue("bt1", 0);
    delay(1000);

    doStart();
  }
  else {
    NextionSetText("t1","MOV not found");
    NextionSetValue("bt0", 0);
    NextionSetValue("bt1", 0);
  }

}

uint32_t frameTime = 0;
uint32_t frameTime_uS = 0;
uint32_t fcount = 0;
int playingaudio=1;
int kstatus = 0;

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, millis()%1000<100);

  if (millis()-frameTime>=10) {
    debounce(digitalRead(HALL_PULSE_PIN));
    frameTime = millis();      
    frameTime_uS = micros();      
    int newstatus = kernelExec(0);
    if (newstatus != kstatus) {
      if (newstatus > 0 ) Serial.println("Kernel active");
      if (newstatus == 0 ) Serial.println("Kernel inactive");
      kstatus = newstatus;
    }
    fcount++;
    if (playingaudio && !playWav1.isPlaying()) {
      Serial.println("Sound finished");
      playingaudio = 0;
    }
  }

  /* Reporting every 100 frames */
  if (fcount==100) {
    if (kstatus>0) {
      int run = GetClockInfo(CLK_RUN);
      NextionShowTime();
      NextionShowActivity();
//        Serial.println(pulse_count);
//        Serial.println(activity);
      if (playWav1.isPlaying()) NextionSetText("t1","Audio Playing");
      else {
        if (run) {
          NextionSetText("t1","Running");
        }
        else {
          NextionSetText("t1","Stopped");
        }
      }
    }
    /*
    Serial.print(micros()-frameTime_uS);
    Serial.print("  ");
    Serial.println();
    */
    fcount = 0;
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c=='s') doStart();
    if (c=='x') doStop();
    if (c=='r') doRewind();
  }

  while (Nextion.available()) {
    uint8_t c = Nextion.read();
    NextionParseEvent(c);
//    Serial.print(c,HEX);Serial.print(" ");
  }

}
