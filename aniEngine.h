/**********************************************************************
 * 
 * Configuration of Anima Engine
 * 
 * defines can be done here or in the Makefile
 * 
 *********************************************************************/

#ifndef aniEngineH
#define aniEngineH

#define NDANCERS     4
#define NDISPLAYS    2

/* Petrusjka dancer */
#define USE_DANCER_PETRUSJKA

/* Teensy sound system */
#define USE_SOUND
#define TSOUND

#include "rmovproc.h"
#include "mbdyproc.h"
#ifdef USE_XDISPLAY
#include "wdspproc.h"
#endif

/* Existing dancer/robot types */
#define DANCER_STD 0
#define DANCER_IRB 1
#define DANCER_RC  2

#ifdef USE_DANCER_DIRIGENT
#define DANCER_SSC  3
#endif

#ifdef USE_DANCER_EZUSB
#define DANCER_EZUSB  4
#endif

#ifdef USE_DANCER_OLYMPIA
#define DANCER_OLYMPIA  5
#endif

#ifdef USE_DANCER_PETRUSJKA
#define DANCER_PETRUSJKA  6
#endif

extern int anima_debuglevel;

class aniEngine {
public:
   aniEngine();
   ~aniEngine();
   int exec(int n) {return kernel->exec(n);};
   void resetAllProc() { kernel->resetAllProc(); };
   int openDancer(int i,unsigned int flag=0);
   int setDancerFile(int i,char * name,VOLID_TYPE * volID);
   int loadDancerFile(int i);
   int setDancerOption(int dancer,const char * option,int value);
   int closeDancer(int i);
   int addProcess(RTProcess * p,int mode,RTProcess * q) { return kernel->addProcess(p, mode, q);};

   int doRedraw(int display,aRect * area);

   rMoveProc * theInStream[NDANCERS];
   mBodyProc * theDancer[NDANCERS];
   char (filename[NDANCERS])[64];
   VOLID_TYPE volid[NDANCERS];
   long filedate[NDANCERS];
#ifdef USE_XDISPLAY
   int openDisplay(int i,DISPLAY_TYPE theDisplayPane,unsigned int flag=0);
   int closeDisplay(int i);
   int linkDancerDisplay(int dancer,int display);
   int unlinkDancerDisplay(int dancer,int display);
   wDisplayProc * theDisplay[NDISPLAYS];
#endif

private:
   RTKernel  * kernel;

};

#endif
