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

/* Use Serial4 on pin32 for Teensy 3.6 */
HardwareSerial * Port = &Serial4;

pololuservo::pololuservo() {
	bufs=bufe=0;
	sb.lasttime=TickCount();
	for (int nr=0;nr<8;nr++){
		servoctrl * servo=&(sb.servo[nr]);
		servo->active=true;
		servo->inupdate=false;
		servo->position=1500<<2;
		servo->target=1500<<2;
		servo->intervall=0;
	}
}

pololuservo::~pololuservo() {
}

void pololuservo::openport(char * portname) {
	Port->begin(57600);
	isopen = true;
	bufs = bufe = 0;
	Message("Open Port");
} 

void pololuservo::set(int nr,int pos) {
		buf[bufe++] = 0x84;
		buf[bufe++] = nr&0x7F;
		buf[bufe++] = pos & 0x7F;
		buf[bufe++] = (pos>>7) & 0x7F;
}

void pololuservo::send() { 
	Port->write(buf,bufe);
	bufe = 0;
}





