/*
// animacpp.c   -   Glue routines to link the C++ RTKernel and process
// code, to a standard C  event and window manager.
// This version only handles one document-window
// The code here is normally found in the AnimaApp.cpp and AnimaDoc.cpp
// modules.
// Model specific globals also live here,
// 91-11-07  Magnus Lundin  CTH,KACOR
// 03-01-30  Magnus Lundin  Added aniEngine object to remove C style globals
*/

#include "anicore.h"

#include "clock.h"
#include "aniEngine.h"
#include <string.h>
#include <stdio.h>
//#include <exception>
#include "aniglue.h"

#ifdef ARDUINO
#include "Arduino.h"
#endif

#ifdef __UNIX__
//#include "sunaudio/aniaudio.h"
#endif
#include <stdlib.h>

//#define VERBOSE

/*  In moveparse.c  */
int Message(const char * msg);

void (* MessageHook)(const char *)  =NULL;

#ifndef __WIN32__
#ifndef Macintosh
int Message(const char * msg) {
/* UNIX systems */
   if (MessageHook) 
   {
   	(*MessageHook)(msg);
   }
   else 
   {
   	Serial.printf("%s\n",msg);
   }
	return 0;
}
#endif
#endif

#ifdef __WIN32__
int Message(const char * msg) {
	if (MessageHook) 
	{
		(*MessageHook)(msg);
	}
	else 
	{
		MessageBox(0,msg,"Anima Message",IDOK);
	}
	return 0;
}

#endif

void  SetMessageHook( void (* fn)(const char *) ) {
   MessageHook=fn;
}

/**********************************************************************/
/* Globals that modify the kernel behaviour */
// Used in agSetColor
extern MATERIAL_TYPE gWorldMaterial,gStageMaterial,gBackdropMaterial;


extern long getFDate(char * fName,VOLID_TYPE * volID);
int checkFileDate(int i) ;    //   Aniglue.c

/***********************************************************************/

void  openAnimaKernel() {
   theEngine=new aniEngine;
#ifdef VERBOSE
   if (theEngine->kernel) Message("Kernel started");
   else  Message("Kernel FAILED");
#endif
}

void  closeAnimaKernel() {
   if (theEngine) delete theEngine;
   theEngine = NULL;
}

int   kernelExec(int n) {
if (theEngine)
	return theEngine->exec(n);
return -1;
}

int   openDancer(int i,unsigned int flag) {
   /**
   * @TODO  HOW TO OPEN DIFFERENT TYPES(SUBCLASSES) OF DANCERS
   */
 //  try {
       return theEngine->openDancer(i,flag);
 //  }
 //  catch (const std::exception &e) {
 //      Message(e.what());
 //  }
 //  return 0;
}

int   closeDancer(int i) {
  if (theEngine)
     return theEngine->closeDancer(i);
   return 0;
}

int   setDancerFile(int i,char * name,VOLID_TYPE * volID){
   theEngine->setDancerFile(i,name,volID);
return 1;
}

int   getDancerFile(int i,char * name){
   strcpy(name,theEngine->filename[i]);
return 1;
}

int   loadDancerFile(int i){
   int res=theEngine->loadDancerFile(i);
   Message("Exit loadDancerFile");
return res;
}

void   autoReloadFiles(void )
{
int k,dirty=0;
   for (k=0;k<NDANCERS;k++)
      if   ((theEngine->theDancer[k]!=NULL) && (checkFileDate(k) > 0) ) {
          loadDancerFile(k);
          dirty=1;
	}
      if (dirty) {      // Restart from 0
//	theEngine->kernel->resetAllProc();
//        theEngine->kernel->exec(0);
      }
}

/* Send a command block to dancer #i */
/* The block is automatically removed after use*/
int    doCommandBlock(int i,char * buf) {
	if ((i<NDANCERS)&& (i>=0))
		if (theEngine->theDancer[i]!=NULL) {
			MoveText * mt=new MoveText();
			mt->copyAsBuffer(buf);         
			mt->autoclean=true;
			theEngine->theDancer[i]->newrMoveProc(mt,DEALLOC | AUTOKILL );
			return 0;
		}
	/* ELSE */
	//free(buf);
	return -1;
}

int   setDancerOption(int i,const char * option,int value) {
   return theEngine->setDancerOption(i,option,value);
}

/******************************************************************************
*
*  Clock and time handling
*
*******************************************************************************/

extern int running, paused; /* In clock.c */
void   doStart(){
    //theEngine->kernel->status=ACTIVE;   /*  ???????????????????????????????? */
    doSetTime(0);
    doStartClock();
}

void   doContinue(void) {
    //theEngine->kernel->status=ACTIVE;
    doStartClock();
}

void   doSetTime(int time){
    //theEngine->kernel->status=ACTIVE;
    int running = isRunning();
    int backward = (time<thisTime) ;
    doStopClock();
    doSetClock(time);
    if (backward)
        theEngine->resetAllProc();
    theEngine->exec(0);
    if (running) {
        doStartClock();    
    }
}

void   doStop(){
    doStopClock();
}

void   doRewind() {
    doSetTime(0L);
}

void   doSetTempo(int newt){
    MM=newt;
    doSyncClock();
}

// INFOFLAG CLK_TIME, CLK_MM, CLK_OFFSET, CLK_MODE
long   GetClockInfo(int  infoflag ){
	switch (infoflag) 
	{
		case CLK_TIME: return thisTime;
		case CLK_MM:   return MM;
		case CLK_OFFSET: return 0;
		case CLK_MODE: return 0;
		case CLK_RUN: return running;
		default: break;
	}
	return 0;
}

void   SetClockInfo(int  infoflag , long value ){
switch (infoflag) {
   case CLK_TIME: //doSetClock(value);
                  doSetTime(value);
                  break;
   case CLK_MM:   doSetTempo(value);
                  break;
   case CLK_OFFSET: break;
   case CLK_MODE: break;
   case CLK_RUN: if (value==1) doContinue();
                 if (value==0) doStop();
   default: break;
   }
}

/******************************************************************************
*
*  Graphics handling
*
*******************************************************************************/

#ifdef USE_XDISPLAY

int  openDisplay(int i,DISPLAY_TYPE theDisplayPane,unsigned int flag) {
   /**
   * @TODO  HOW TO OPEN DIFFERENT TYPES(SUBCLASSES) OF DISPLAYS
   */
   return theEngine->openDisplay(i,theDisplayPane,flag);
}

int   closeDisplay(int i) {
  if (theEngine)
     return theEngine->closeDisplay(i);
   return 0;
}

int   linkDancerDisplay(int dancer,int display){
    return theEngine->linkDancerDisplay(dancer,display);
   return 0;
}

int   unlinkDancerDisplay(int dancer,int display){
    return theEngine->unlinkDancerDisplay(dancer,display);
}

int   doRedraw(int display,aRect * area) {
	if (theEngine)
		return theEngine->doRedraw(display,area);
	return 0;

}

void   agSetColor(int dancer,int colorpart,short r,short g,short b){
RGBCOLOR * c=(RGBCOLOR *)NULL;
   if (colorpart>=0 && colorpart<4)
   if (theEngine->theDancer[dancer]!=NULL)
      c=&theEngine->theDancer[dancer]->attribs.material[colorpart].color;
   if (colorpart==-1) c=&(gWorldMaterial.color);
   if (colorpart==-2) c=&(gStageMaterial.color);
   if (colorpart==-3) c=&(gBackdropMaterial.color);
   if (c==NULL) return;
   c->r=r&255;
   c->g=g&255;
   c->b=b&255;
   theEngine->doRedraw(0,(aRect *)NULL);
}

/******************************************************************************
*
*  Camera handling
*
*******************************************************************************/

void   moveCamera(int display,int dx,int dy) {
	wDisplayProc * theDisplay = theEngine->theDisplay[display];
	if (theDisplay != NULL) 
	{
		theDisplay->aCameraPos.Pan+=(float)dx/360.0;
		theDisplay->aCameraPos.Tilt+=(float)dy/360.0;
		theDisplay->drawflag=DRAW_ALL;
		theEngine->doRedraw(display,(aRect *)NULL);
	}
 }

void   zoomCamera(int display,int dz) {
	wDisplayProc * theDisplay=theEngine->theDisplay[display];
	if (theDisplay != NULL) 
	{
		theDisplay->aCameraPos.Fov*=(1.0+(float)dz/1000.0);
		theDisplay->drawflag=DRAW_ALL;
		theEngine->doRedraw(display,(aRect *)NULL);
	}
}

void   distanceCamera(int display,int dz) {
	wDisplayProc * theDisplay=theEngine->theDisplay[display];
	if (theDisplay != NULL) 
	{
		theDisplay->aCameraPos.Position.z +=dz;
		theDisplay->drawflag=DRAW_ALL;
		theEngine->doRedraw(display,(aRect *)NULL);
	}
}


#endif

/* NOT USED
void   SetCamera(int display,CAMERAREC * newCameraData) {
if (theDisplay[display])
    {
 	theDisplay[display]->aCameraPos=*newCameraData;
        doRedraw(display,(Rect *)NULL);
    }
}
*/

/******************************************************************************
*
*  Misc perhaps useful routines
*
*******************************************************************************/

// NONEXPORTED UTILITY FUNCTIONS

int checkFileDate(int i)     //   Aniglue.c
{
//  New different date returns > 0
long newDate;
   setVolume(volid[i]);
   newDate=getFDate(theEngine->filename[i],&theEngine->volid[i]);
   if (newDate) return !(newDate == theEngine->filedate[i]);
   else return -1;
}

