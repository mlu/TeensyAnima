#include "ssc.h"
#include "clock.h"

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>


/***************************************************************************
*
*   class rcServoController
*   Generic servo handling
*
****************************************************************************/
#define SERVOMAX 8
//rcServoController * gservolist[SERVOMAX]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
static bool ginupdate = false;

bool rcServoController::isupdating = false;

rcServoController::rcServoController() {
	servoblocksize = 8;
	isopen = false;
	addServo(this);
}

rcServoController::~rcServoController() {
	removeServo(this);
}

void rcServoController::moveto(int nr, int pos, int intervall) {
	servoctrl * servo=&(sb.servo[nr]);
	servo->inupdate=true;
	servo->active=true;
	servo->target=pos;
	servo->intervall=intervall;
	servo->inupdate=false;
	char buf[64];
	sprintf(buf,"moveto nr=%i pos=%hi targ=%hi int=%i",nr, servo->position,servo->target,servo->intervall);
	Message((char*) buf);
}

void rcServoController::updateservopos(){
	//
	long dt;
	// UNIXTIME
	long t0=TickCount();

	dt=t0-sb.lasttime;
	sb.lasttime=t0;
	for (int k=0;k<servoblocksize;k++) {
		servoctrl * servo=&(sb.servo[k]);
		if (servo->active && !servo->inupdate) 
		{
			//printf("ssc:updatesb pos=%hi targ=%hi int=%i dt=%li\n",servo->position,servo->target,servo->intervall, dt);
			int pos,pos0=(servo->position)>>8;
			if (dt>=servo->intervall) 
			{
				servo->position=servo->target;
				servo->intervall=0;
				servo->active=false;
			}
			else 
			{
				int delta=((servo->target-servo->position)*dt)/servo->intervall;
				servo->intervall -= dt;
				servo->position += delta;
			}
			pos=(servo->position)>>8;
			if (pos != pos0)
			{
				set(k,pos);
			}
		}
	}
	send();
}

void rcServoController::addServo(rcServoController * servo) 
{
	for (int i=0;i<SERVOMAX;i++) 
	{
		if (gservolist[i]==NULL) 
		{
			 gservolist[i]=servo;
			 return;
		}
	}
}

void rcServoController::removeServo(rcServoController * servo) 
{
	for (int i=0;i<SERVOMAX;i++) 
	{
		if (gservolist[i]==servo) 
		{
			 gservolist[i]=NULL;
			 return;
		}
	}
}

void updateaction(int arg) {
	if (ginupdate) return;
	ginupdate = true;
	for (int i=0;i<SERVOMAX;i++) 
	{
		if (gservolist[i]!=NULL) 
		{
			 gservolist[i]->updateservopos();
		}
	}
	ginupdate = false;
}


void rcServoController::startupdate() {
#if defined( __unix__ ) || defined( __linux__ )
	isupdating=true;
	itimerval timervalue;
	timervalue.it_interval.tv_sec=0;
	timervalue.it_interval.tv_usec=20000;
	timervalue.it_value.tv_sec=0;
	timervalue.it_value.tv_usec=20000;
	signal(SIGALRM,updateaction);
	setitimer(ITIMER_REAL,&timervalue,NULL);
#endif
}

void rcServoController::stopupdate() {
#if defined( __unix__ ) || defined( __linux__ )
	itimerval timervalue;
	timervalue.it_interval.tv_sec=0;
	timervalue.it_interval.tv_usec=0;
	timervalue.it_value.tv_sec=0;
	timervalue.it_value.tv_usec=0;
	signal(SIGALRM,SIG_DFL);
	setitimer(ITIMER_REAL,&timervalue,NULL);
	isupdating = false;
#endif
}

