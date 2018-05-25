#ifndef _ANIGLUE_H
#define _ANIGLUE_H
/* Definitions and prototypes for aniglue */

#ifdef USE_XDISPLAY
#include "sysdisplaytype.h"
#endif
#include "anisysd.h"
#include "aniEngine.h"

#define WORLDCOLORIX  -1
#define STAGECOLORIX  -2
#define BACKDROPCOLORIX  -3
#define SKINCOLORIX  0
#define LEGCOLORIX   2
#define TORSOCOLORIX 3

#define CLK_TIME 1
#define CLK_MM   2
#define CLK_OFFSET 4
#define CLK_MODE 8
#define CLK_RUN 16

#define DISPLAY_STD 0
#define DISPLAY_GL  1

/* Dancer type numbers are defined in aniEngine.h */

#ifdef __cplusplus
extern "C" {
#endif

void  openAnimaKernel(void);
void  closeAnimaKernel(void);
void  SetMessageHook( void (* fn)(const char *) );
int   kernelExec(int);

int   openDancer(int i,unsigned int flag);
int   closeDancer(int i);

int   setDancerFile(int i,char * name,VOLID_TYPE * volID);
int   getDancerFile(int i,char * name);
int   loadDancerFile(int i);
void  autoReloadFiles(void );
int   doCommandBlock(int i,char * buf);
int   setDancerOption(int i,const char * option,int value);

void  doStart(void);
void  doStop(void);
void  doContinue(void);
void  doSetTime(int time);
void  doRewind(void);
void  doSetTempo(int mm);
long  GetClockInfo(int  infoflag );
void  SetClockInfo(int  infoflag , long value );   // DATAFLAG CLK_TIME, CLK_MM, CLK_OFFSET, CLK_MODE

#ifdef USE_XDISPLAY
int   openDisplay(int i,DISPLAY_TYPE theDisplayPane,unsigned int flag);
int   closeDisplay(int i);
int   linkDancerDisplay(int dancer,int display);
int   unlinkDancerDisplay(int dancer,int display);
int   doRedraw(int display, aRect * area);
void  agSetColor(int dancer,int colorpart,short r,short g,short b);

void  moveCamera(int dpy,int dx,int dy);
void  zoomCamera(int dpy,int dz);
void  distanceCamera(int dpy,int dz);
#endif

#ifdef __cplusplus
}
#endif


#endif







