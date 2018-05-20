#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "anicore.h"
#include "movecmd.h"
#include "movetext.h"
#include "clock.h"

#include "rmovproc.h"

#ifdef USE_SOUND
#ifdef ASOUND
#include "asoundproc.h"
#endif
#ifdef PSOUND
#include "psoundproc.hgg"
#endif
#ifdef TSOUND
#include "tsoundproc.h"
#endif
#endif


extern int GetMoveString(MoveTextCursor * aText,MOVEREC * aMoveRec);
extern int GetMoveInteger(MoveTextCursor * aText,MOVEREC * move);
extern int addToken(TOKENELEMENT ** param);
extern char ** filename;



void SayHello(rMoveProc * reader,char * msg);
#ifdef USE_SOUND
void playSound(rMoveProc * reader,char * sndfile);
#endif
void loadLib(MoveTextCursor * aText);
void initExtfun(void);
void colorCmd(MoveTextCursor * aText);

void SayHello(rMoveProc * reader,char * msg) {
	Message("Hello");
}


extern void    loadMIDIcmdfile(char * fname);

void initExtfun(void) {
installFun((char *)"HELLO",VFuncS,(long)SayHello);
#ifdef USE_SOUND
installFun((char *)"PLAYSOUND",VFuncS,(long)playSound);
#endif
installFun((char *)"LOADLIB",VFuncR,(long)loadLib);
installFun((char *)"COLOR",VFuncR,(long)colorCmd);
#ifdef Macintosh
installFun((char *)"LOADMIDICMD",VFuncS,(long)loadMIDIcmdfile);
#endif
}

extern int findFile( char *fName,VOLID_TYPE * fileSpec);

void loadLib(MoveTextCursor * aText) {
int dnr;
char str[100],*fname,*basefile;
VOLID_TYPE fileSpec, oldVol;
short err;
MOVEREC aMoveRec;
    GetMoveString(aText,&aMoveRec);
    fname=(char *)(aMoveRec.value);
    dnr=theEngine->openDancer(-1);
    if (dnr >=0) {
       getVolume(&oldVol);
       err=findFile( fname, &fileSpec );
       theEngine->setDancerFile(dnr,fname,&fileSpec);
       if (theEngine->loadDancerFile(dnr)==0) {
           sprintf(str,"Failed library load: %s",fname) ;
	   Message(str);
       }
       theEngine->closeDancer(dnr);
       setVolume(oldVol);
    }
    free(fname);
}


//extern RGBCOLOR gWorldColor,gStageColor,gBackdropColor;

void colorCmd(MoveTextCursor * aText){
#ifdef USE_XDISPLAY
int n,r,g,b;
MOVEREC aMoveRec;
int dancer;
	GetMoveInteger(aText,&aMoveRec);
	n=aMoveRec.value;
	GetMoveInteger(aText,&aMoveRec);
	r=aMoveRec.value;
	GetMoveInteger(aText,&aMoveRec);
	g=aMoveRec.value;
	GetMoveInteger(aText,&aMoveRec);
	b=aMoveRec.value;
        dancer=((rMoveProc *)(aText->getOwner()))->theBody->refnr;
	agSetColor(dancer,n,r,g,b);
#endif
}


#ifdef USE_SOUND
void playSound(rMoveProc * reader,char * sndfile) {
   long parentTime;
   parentTime=reader->EventTime;
#ifdef PSOUND
   pSoundProc * sp = new pSoundProc(sndfile);
#endif
#ifdef ASOUND
   AlsaSoundProc * sp = new AlsaSoundProc(sndfile);
#endif
#ifdef TSOUND
   TeensySoundProc * sp = new TeensySoundProc(sndfile);
#endif
   sp->startTime=parentTime;
   sp->reader = reader;
   theEngine->addProcess(sp,AFTER,sp);
   char buf[64];
   sprintf(buf,"pSoundProc created at %li  start %li\n",thisTime,parentTime);
   Message(buf);
};
#endif
