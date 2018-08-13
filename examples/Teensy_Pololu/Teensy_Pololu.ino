#include <Servo.h>


Servo servo[8];
int servopin[8] = {2, 3, 4, 5, 6, 7, 8, 9};

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

  for (int nr=0;nr<8; nr++) {
    servo[nr].attach(servopin[nr], 800, 2200); 
  }

  Serial1.begin(57600);

}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial1.available()) {
    ParseServoCmd(Serial1.read());
  }
}

