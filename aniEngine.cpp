#include "aniEngine.h"
#include "kernel.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <stdexcept>
#include "clock.h"
#include "mrobproc.h"
#include "mrcrobot.h"

#ifdef USE_DANCER_DIRIGENT
#include "mminisscrobot.h"
#endif

#ifdef USE_DANCER_OLYMPIA
#include "molympiarobot.h"
#endif

#ifdef USE_DANCER_EZUSB
#include "mrcezusb.h"
#endif

#ifdef USE_DANCER_PETRUSJKA
#include "mpetrusjka.h"
#endif

#ifdef USE_OPENGL
#include "wdspopengl.h"
#endif

int anima_debuglevel = 1;

extern void initExtfun(void);
extern void initCorefun(void);
extern long getFDate(char * fName,VOLID_TYPE * volID);
extern int  checkFileDate(int i) ;    //   Aniglue.c

aniEngine::aniEngine(){
   kernel=new( RTKernel);
   for (int k=0;k<NDANCERS;k++) {
      theInStream[k]=(rMoveProc *)NULL;
      theDancer[k]=(mBodyProc *)NULL;
   };
#ifdef USE_XDISPLAY
   for (int k=0;k<NDISPLAYS;k++) theDisplay[k]=(wDisplayProc *)NULL ;
#endif
   doSetClock(0);
   kernel->status=KS_READY;
   kernel->resetAllProc();
   initExtfun();
   initCorefun();
};

aniEngine::~aniEngine() {
   int k;
   for (k=0;k<NDISPLAYS;k++)
#ifdef USE_XDISPLAY
      if (theDisplay[k])
         delete theDisplay[k];
#endif
   kernel->resetAllProc();
};

int  aniEngine::openDancer(int i,unsigned int flag) {
bool newdancer=false;
bool newinstream=false;
	if (i<0) 
	{       /* find next unused dancer */
	        int k=0;
	        while ((k<NDANCERS)&&(theDancer[k]!=NULL)) k++;
	        if (k==NDANCERS) return -1;
		    i=k;
	}
	if ((theDancer[i]==NULL)&&(i<NDANCERS)) 
	{
		  newdancer=true;
		  switch(flag) {
		  case DANCER_STD:
		    theDancer[i]=new mBodyProc();
		    break;
		    /*
		  case DANCER_RC:
		    theDancer[i]=new mRCRobotProc();
		    break;
		    */
#ifdef USE_DANCER_DIRIGENT
		  case DANCER_SSC:
		    theDancer[i]=new mMiniSscRobotProc();
		    break;
#endif
#ifdef USE_DANCER_EZUSB
		  case DANCER_EZUSB:
		    theDancer[i]=new mRCEzUSBProc();
		    break;
#endif
#ifdef USE_DANCER_PETRUSJKA
		  case DANCER_PETRUSJKA:
		    theDancer[i]=new mPetrusjkaProc();
		    break;
#endif
#ifdef USE_DANCER_OLYMPIA
		  case DANCER_OLYMPIA:
		    theDancer[i]=new mOlympiaRobotProc();
		    break;
#endif
		  default:
//		    throw  "Unsupported dancer type in aniEngine";
			Message("Unsupported dancer type in aniEngine");
			return -1;
		  }
	}
	if (theInStream[i]==NULL) 
	{
		newinstream=true;
		theInStream[i]=new(rMoveProc);
	}
	theInStream[i]->theBody=theDancer[i];
	theDancer[i]->refnr=i;
	theInStream[i]->startTime=0;
	if (newdancer) kernel->addProcess(theDancer[i],TOP,(RTProcess *)NULL);
	if (newinstream) kernel->addProcess(theInStream[i],TOP,(RTProcess *)NULL);
	return i;
}

int  aniEngine::setDancerFile(int i,char * name,VOLID_TYPE * volID){
	if (name) strncpy(filename[i],name,62);
	if (volID) volid[i]=*volID;
	return 1;
}

int  aniEngine::loadDancerFile(int i){
	MoveText * mt;

	if (i<0) return 0;
	if (theDancer[i]==NULL) openDancer(i);
    setVolume(volid[i]);
    mt=InitFile( filename[i] ,&volid[i] );
    if (mt) {
       filedate[i]=getFDate(filename[i],&volid[i]);
       theInStream[i]->theText.setText(mt);
       theInStream[i]->modeflag = DEALLOC;
       theInStream[i]->reset();
       theInStream[i]->exec();
       theDancer[i]->exec();
       return 1;
	}
    else filedate[i]=0;
return 0;
}

int  aniEngine::setDancerOption(int dancer,const char * option,int value){
   if ((dancer<0)||(dancer>=NDANCERS)) return -1;
   return theDancer[dancer]->setOption(option,value);
}

int  aniEngine::closeDancer(int i) {
   if ((i<0)||(i>=NDANCERS))return 0;
   if (theInStream[i]==NULL) return 0;
   // Search all displays for this dancer
#ifdef USE_XDISPLAY
   for (int l=0;l<NDISPLAYS;l++)
	bool found;
      if (theDisplay[l]!=NULL) {
         found=false;
         for (int k=0;k<theDisplay[l]->nDancers;k++) {
            if (theDisplay[l]->dancers[k]==theDancer[i])
               found=true;
            if (found) theDisplay[l]->dancers[k]=theDisplay[l]->dancers[k+1];
            }
         if (found)
            theDisplay[l]->nDancers--;
      }
#endif
   theInStream[i]->clean();
   theInStream[i]=(rMoveProc *)NULL;
   theDancer[i]->clean();
   theDancer[i]=(mBodyProc *)NULL;
return 1;
}

#ifdef USE_XDISPLAY
int aniEngine::openDisplay(int i,DISPLAY_TYPE theDisplayPane,unsigned int flag){
   /**
   * @TODO  HOW TO OPEN DIFFERENT TYPES(SUBCLASSES) OF DISPLAYS
   */
   if (i<0) {       /* find next unused display */
        int k=0;
        while ((k<NDISPLAYS)&&(theDisplay[k]!=NULL)) k++;
        if (k==NDISPLAYS) return -1;
        i=k;
   }
   if (theDisplay[i]==NULL)
     switch(flag) {
        case DISPLAY_STD:
           theDisplay[i]=new wDisplayProc();
           break;
#ifdef USE_OPENGL
        case DISPLAY_GL:
           theDisplay[i]=new wDisplayOpenGl();
           break;
#endif
     default:
       throw "Unsupported display type in aniEngine";
     }
   theDisplay[i]->init();
   theDisplay[i]->setDisplay(theDisplayPane);
   theDisplay[i]->nDancers=0;
   kernel->addProcess(theDisplay[i],BOTTOM,0L);
return i;
};

int  aniEngine::linkDancerDisplay(int dancer,int display){
int k,found=0;
    if ((dancer < 0) || (dancer >= NDANCERS)) return -1;
    if ((display < 0) || (display >= NDISPLAYS)) return -1;
    if (theDancer[dancer]==NULL) return -1;
    if (theDisplay[display]==NULL) return -1;

    /* Dont link the same dancer twice */
    for (k=0;k<theDisplay[display]->nDancers;k++)
	     found |= (theDisplay[display]->dancers[k]==theDancer[dancer]);
    if (! found )  theDisplay[display]->dancers[theDisplay[display]->nDancers++]=theDancer[dancer];
//    if (kernel->status != ACTIVE) theDisplay[display]->exec();
    if ((kernel->status != KS_READY) && !isRunning() ) theDisplay[display]->exec();
    return 0;
}

int  aniEngine::unlinkDancerDisplay(int dancer,int display){
int k;
bool found;
   if ((dancer<0)||(dancer>=NDANCERS))return -1;
   if (theInStream[dancer]==NULL) return -1;
   if (theDisplay[display]==NULL) return -1;
   // Search the displays for this dancer
   found=false;
   for (k=0;k<theDisplay[display]->nDancers;k++) {
	  if (theDisplay[display]->dancers[k]==theDancer[dancer]) found=true;
	  if (found) theDisplay[display]->dancers[k]=theDisplay[display]->dancers[k+1];
   }
   if (found)
      theDisplay[display]->nDancers--;
return 0;
}

int  aniEngine::closeDisplay(int i) {
   if ((i<0)||(i>=NDANCERS))return 0;
   if (theDisplay[i]==NULL) return 0;
   //printf("closeDisplay: cleab( %li ) \n",theDisplay[i]);
   theDisplay[i]->clean();
   theDisplay[i]=(wDisplayProc *)NULL;
return 1;
}

int  aniEngine::doRedraw(int display,aRect * area) {
if ((display>=0)&&(display<NDISPLAYS))
   if (theDisplay[display]) {
      if (area==NULL)
	theDisplay[display]->drawflag=DRAW_ALL;
      if (kernel->status == KS_READY) {
      if (isRunning())
          kernel->exec(0);
        else
	  theDisplay[display]->draw(area);
      }
      return 0; // Valid display
   }
 return -1; // Not a valid display
}

#endif
