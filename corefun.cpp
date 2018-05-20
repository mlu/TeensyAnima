#include "movelib.h"
#include "movecmd.h"
#include "anicore.h"
#include "clock.h"
#include <string.h>
#include <stdio.h>

//extern TOKENELEMENT * nextToken(MoveTextCursor * aText);
extern int  GetMoveInteger(MoveTextCursor * aText,MOVEREC * move );
extern long offsetTime;

void initCorefun(void);
void setTimeCmd(long value);
void tempoCmd(long value);
void setBeginCmd(MoveTextCursor * aText);
void traceCmd(MoveTextCursor * aText);
void stagesizeCmd(MoveTextCursor * aText);
long plusCmd(long,long);
void setOffsetCmd(long value);


void initCorefun(void) {
   installFun((char *)"SETCLOCK",VFuncI,(long)setTimeCmd);
   installFun((char *)"SETTIMEOFFSET",VFuncI,(long)setOffsetCmd);
   installFun((char *)"TEMPO",VFuncI,(long)tempoCmd);
   installFun((char *)"BEGIN",VFuncR,(long)setBeginCmd);
   installFun((char *)"TRACE",VFuncR,(long)traceCmd);
#ifdef USE_XDISPLAY
   installFun((char *)"STAGESIZE",VFuncR,(long)stagesizeCmd);
#endif
   installFun((char *)"PLUS",IFuncII,(long)plusCmd);
}

void setTimeCmd(long value){
	doSetClock(value);
}

void setOffsetCmd(long value){
	offsetTime=value;
}

void tempoCmd(long value){
	doSetMetronome((int)(value));
}

void setBeginCmd(MoveTextCursor * aText)
{
// Skip text/tokens until we return to original level
int startLevel=aText->getLevel();
	while (aText->getLevel()>=startLevel)
           aText->nextToken();
	aText->setBufferStart(aText->getPosition());
}

void traceCmd(MoveTextCursor * aText){
	const char *  st,* sl;
	char buf[52],outs[100];
	int l,startLevel;

	startLevel=aText->getLevel();
	st=aText->getPosition();
	sl=st;
	while (aText->getLevel()>=startLevel) {
		// Skip text/tokens until we return to original level
		sl=aText->getPosition();
		aText->nextToken();
	}
	l=sl-st;
	if (l>50) l=50;
	strncpy(buf,st,l);
	buf[l]=0;
	sprintf(outs,"Trace %li:%s ",thisTime,buf);
	Message(outs);
}

#ifdef USE_XDISPLAY
extern long gStageXmin;
extern long gStageXmax;
extern long gStageZmin;
extern long gStageZmax;

void stagesizeCmd(MoveTextCursor * aText){
   MOVEREC aMoveRec;
   GetMoveInteger(aText,&aMoveRec);
   gStageXmin=10*aMoveRec.value;
   GetMoveInteger(aText,&aMoveRec);
   gStageXmax=10*aMoveRec.value;
   GetMoveInteger(aText,&aMoveRec);
   gStageZmin=10*aMoveRec.value;
   GetMoveInteger(aText,&aMoveRec);
   gStageZmax=10*aMoveRec.value;
   theEngine->doRedraw(0,NULL);
}
#endif

long plusCmd(long a,long b){
   return a+b;
}




