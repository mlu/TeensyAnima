#include <Servo.h>


Servo servo[8];
int servopin[8] = {12, 10, 9, 8, 7, 6, 5, 4};

/*****************************************************

Petrusjka servo settings from mpetrusjka.cpp
                        min   max   zero  dir    (0.25ms)
SERVOPROP petr_servoprop[8]={
   {(char*)"RKNEE V",  4720, 7520, 4720,  1, 93.6},
   {(char*)"RHIP V",   4800, 7520, 7520, -1, 23.4},
   {(char*)"LKNEE V",  4480, 3760, 3760, -1, 23.4},
   {(char*)"LHIP V",   4640, 7520, 4640,  1, 23.4},
   {(char*)"UPPERBACK",4160, 7680, 4160,  1, 69.2},
   {(char*)"RARM V",   4240, 8080, 8080, -1, 29.6},
   {(char*)"LARM V",   4240, 8080, 4240,  1, 29.6},
   {(char*)"NONE",     0, 0, 0, 0, 0}
};

- Bestäm neutral läge, till kolumn 3
- Bestäm maximalt utslag 
SERVOPROP petr_servoprop[8]={
   {(char*)"RKNEE V",  4720, 7520, 7520, -1, 93.6}, prel
   {(char*)"RHIP V",   4800, 7500, 7500, -1, 23.4}, 
   {(char*)"LKNEE V",  4760, 6760, 4760,  1, 23.4}, prel
   {(char*)"LHIP V",   4640, 7520, 4540,  1, 23.4},
   {(char*)"UPPERBACK",4160, 7680, 4160,  1, 69.2},
   {(char*)"RARM V",   4240, 8080, 8080, -1, 29.6},
   {(char*)"LARM V",   4240, 8080, 4240,  1, 29.6},
   {(char*)"NONE",     0, 0, 0, 0, 0}
};

*****************************************************/
uint32_t petr_servozero[8]={
   7020,
   7500,
   4560,
   4540,
   4160,
   8080,
   4080,
   0000
};


/*****************************************************
* 
* Servo Control Parser
* 
* Pololo Compact https://www.pololu.com/docs/0J40/5.c
* 
*****************************************************/

#define CMD_SET_TARGET 0x84
#define CMD_SET_MULTIPLE_TARGETS 0x9F
#define CMD_SET_SPEED 0x87
#define CMD_SET_ACCEL 0x89

uint8_t cmdbuf[32];
uint32_t cmdbufpos = 0;

void ParseServoCmd(uint8_t ch) {
  if (ch & 0x80) {
    cmdbuf[0] = ch;
    cmdbufpos = 1;
  }
  else {
    cmdbuf[cmdbufpos++] = ch;
    if (cmdbufpos >= 32) {
      cmdbufpos = 0;
      return;
    }
  }

  if (cmdbuf[0]==CMD_SET_TARGET) {  /* value*0.25uS */ 
    if (cmdbufpos >= 4) {
      uint32_t channel = cmdbuf[1];
      uint32_t value = (cmdbuf[2]&0x7F) | ((cmdbuf[3]<<7)&0x3F80);
//      servo[channel].setTarget(value);
      servo[channel].writeMicroseconds(value>>2);
      cmdbufpos = 0;
    }
  }

  if (cmdbuf[0]==CMD_SET_MULTIPLE_TARGETS) {  /* value*0.25uS */ 
    if (cmdbufpos >= 5) {
      if (cmdbuf[1]>0) {
        uint32_t channel = cmdbuf[2];
        uint32_t value = (cmdbuf[3]&0x7F) | ((cmdbuf[4]<<7)&0x3F80);
//        servo[channel].setTarget( value );
        servo[channel].writeMicroseconds(value>>2);
        cmdbufpos = 3;
        cmdbuf[1]--;
        cmdbuf[2]++;
      }
    }
    else {
      cmdbufpos = 0;
    }
  }

}

/*****************************************************
* 
* Application and main loop
* 
* Pololo Compact https://www.pololu.com/docs/0J40/5.c
* 
*****************************************************/

void setup() {

  for (int nr=0;nr<7; nr++) {
    servo[nr].attach(servopin[nr], 800, 2200); 
    servo[nr].writeMicroseconds(petr_servozero[nr]>>2);
  }

  Serial1.begin(57600);

}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial1.available()) {
    ParseServoCmd(Serial1.read());
  }
}

