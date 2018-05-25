#include "pololuservo.h"
#include "movelib.h"
#include "clock.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include <unistd.h>
#include <assert.h>

#ifdef ARDUINO
#include "Arduino.h"
#endif

/***************************************************************************
*
*   class pololuservo
*   Control of pololu based RC servo controller
*
****************************************************************************/

pololuservo::pololuservo() {
	bufs=bufe=0;
	sb.lasttime=TickCount();
	for (int nr=0;nr<8;nr++){
		servoctrl * servo=&(sb.servo[nr]);
		servo->active=true;
		servo->inupdate=false;
		servo->position=127<<8;
		servo->target=128<<8;
		servo->intervall=0;
	}
}

pololuservo::~pololuservo() {
}

void pololuservo::openport(char * portname) {
	Serial1.begin(9600);
	isopen = true;
	bufs = bufe = 0;
}

void pololuservo::set(int nr,int pos) {
  if (((bufe+8)%POLOLUBUFSIZE) != bufs) {
    buf[bufe] = 0x84;
    buf[bufe+1] = nr&0x7F;
    buf[bufe+2] = pos & 0x7F;
    buf[bufe+3] = (pos>>7) & 0x7F;
    bufe = (bufe+4)%POLOLUBUFSIZE;
  }
}

void pololuservo::send() { 
	Serial1.write(buf,bufe);
	bufe = 0;
}





