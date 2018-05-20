#include "clock.h"
#include "anisysd.h"
//#include <string.h>
#define VERBOSE


#ifdef Macintosh
#define USE_MIDI
/***************************   MIDI MANAGER CODE only ON MAC
Flyttad till macmidi.c   8/8 93 Magnus
******************/
extern long getMIDITime(void);
extern void setMIDITime(long newtime);
extern void playMetronome(void);
extern void stopMetronome(void);

extern int gMetronome;

#endif


#ifdef __UNIX__
//#include "aniaudio.h"
#endif

int MM=60,framecount=0;
int frameRate=15;
long nextTime;
int tick_per_frame=20;
int running=0, paused=0;  /* Program activity status */

int gClockMode=INTERNALCLOCK;

/*  Millisecond clock */
#if defined( __unix__ ) || defined( __linux__ ) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/time.h>

struct timezone timez;
struct timeval timev;
long s0;

/* Base millisecond clock */
long TickCount() {
   long tc;
   if (s0 == 0) {     /* Initiate */
	gettimeofday(&timev,&timez);
	s0=timev.tv_sec;
	};
   gettimeofday(&timev,&timez);
   tc=(timev.tv_sec-s0)*1000+(timev.tv_usec/1000);
   return tc;
}
#endif

#ifdef __WIN32__
#include <windows.h>
long TickCount() {
   return GetCurrentTime();
}
//#define TickCount GetCurrentTime
#endif

#ifdef ARDUINO
#include "Arduino.h"
long TickCount() {
   return millis();
}
#endif
/**************************************************************************/
long thisTime=0,baseTime=0,offsetTime=0;
long fixedFrameClock=0; /* 600 ticks per sec */
long internalTime=0;

long lastTick=0;


int doClock(void) {
   thisTime=getTime()-offsetTime;
   lastTick=TickCount();
   return 0;
}


long  getTime(void) {
	if (running && !paused) {
#ifdef Macintosh
		/* Absolute time */
		switch (gClockMode) {
			case INTERNALCLOCK:
		   		internalTime=( (long)MM  * (TickCount()-baseTime) )/(6*6);
		   		break;
#ifdef USE_MIDI
		   	case MIDIBEAT:    /*   MIDI beat clock, 960 ??  ticks per beat */
		   		internalTime=getMIDITime()*5/(48);
		   		if (internalTime > thisTime) MM=((internalTime-thisTime)*36)/(TickCount()-lastTick);
		   		break;
		   	case MIDITIMECODE:   /*   MIDI absolute time in millisec's */
		   		internalTime=(MM*getMIDITime())/(600);
		   		break;
#endif
			case FIXED_RATE:
			    fixedFrameClock+=(600/frameRate);
		   		internalTime=( (long)MM  * (fixedFrameClock) )/(60*6);
		   		break;
		}
#else
		   		internalTime=( MM * (TickCount()-baseTime) )/(10*60);
#ifdef __UNIX__
//		        playSound();
#endif
#endif
	}
	return internalTime;
}


/***************************************************************************/
int doStartClock(void) {
	running=1;
	paused=0;
	    /*  real time sync code */
#ifdef __UNIX__
//   if (sound_flag) sound_flag=SOUND_PLAY;
//   playSound();
#endif

	doSyncClock();
return 0;
}

int doStopClock(void) {
	running=0;
	paused=1;
	    /*  real time sync code */
#ifdef __UNIX__
//   if (sound_flag) sound_flag=SOUND_PAUSE;
#endif
return 0;
}

int isRunning() { return running; }
/***************************************************************************/
int doSetClock(long newTime) {
    /* Absolute time */
    internalTime=newTime;
    doSyncClock();
    return 0;
}

/***************************************************************************/
int doSetMetronome(int newMM) {
/* Absolute time */
if (newMM>0) MM=newMM;
else MM=1;
/*printf("doSetClock %li \n",internalTime);*/
doSyncClock();
return 0;
}

/****************************************************************************/

int doSetClockMode(int newMode) {
#ifdef Macintosh
		/* Absolute time */
		switch (newMode) {
			case INTERNALCLOCK:
			      break;
#ifdef USE_MIDI
		   	case MIDIBEAT:    /*   MIDI beat clock, 960 ??  ticks per beat */
		   	case MIDITIMECODE:   /*   MIDI absolute time in millisec's */
#endif
			case FIXED_RATE:
		   	      break;
			default: return gClockMode;
		}
#else

#endif
    gClockMode=newMode;
    doSyncClock();
    return gClockMode;
}

/****************************************************************************/
int doSyncClock(void) {
#ifdef Macintosh
#ifdef USE_SOUND
if (gMetronome) playMetronome(); // CHANGE TO setSoundPos(long millisecTime);
#endif
switch (gClockMode) {
	case INTERNALCLOCK:
		baseTime=TickCount()- ( 3600 * internalTime )/(MM*100);
   		break;
		case FIXED_RATE:
	   		fixedFrameClock=6000*6*internalTime/((long)MM*100);
   		break;
#ifdef USE_MIDI
   	case MIDIBEAT:
   		setMIDITime(internalTime*48/5);
#endif
}
#else
/*  Drag bort en extra 1 for avrundning at ratt hall */
		baseTime=TickCount()- ( 600 * internalTime )/(MM)-1;
#endif
#ifdef __UNIX__
//setSoundPos(internalTime*10);
#endif
return 0;
}


