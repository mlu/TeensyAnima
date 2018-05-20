/*********************************************************************************
Real time kernel

Processes for MOVE
*********************************************************************************/
#include "rmovproc.h"
#include "movelib.h"
#include "movecmd.h"
#include "clock.h"
#include <stdio.h>
#include <limits.h>

#ifdef THINK_C
#include <oops.h>
#include <stdlib.h>
#else
//#include <malloc.h>
#include <stdlib.h>
#endif
extern long thisTime;
extern int frcount;

char outs[100];

/*********************************************************************************
rMoveProc process class


*********************************************************************************/

int rMoveProc::activate(void){
    theText.reset();
//	nextTime=((thisTime-startTime)*localSpeed)/100 ;
    nextTime=EventTime=startTime;//thisTime;
    status=ACTIVE;
    priority=0;
    return 0;
}

int rMoveProc::reset(void){
   if ((modeflag & AUTOKILL) ) {
      clean();
      return 0;
   }
   theBody->reset();
   theText.reset();
// nextTime=((thisTime-startTime)*localSpeed)/100 ;
//   nextTime=EventTime=thisTime;
   nextTime=EventTime=startTime;
   status=ACTIVE;
   readFlag=0;
return 0;
}



int rMoveProc::exec(void){
MOVEREC move[40];
/* if (running && !paused) */
   if (!theText.isLoaded()) return 0;
   if (thisTime < EventTime) /* return */ reset();
   //if ( !buffer->isActive()) {clean();return 0;}
   if ( theBody->status == QUIT) { clean(); return 0; };

   NextLocalEventTime=((nextTime-startTime)*localSpeed)/100 ;
   //  lastTime=thisTime;
   while ( nextTime <= thisTime) { //-startTime)*localSpeed)/100 ){
	     /*	 LocalEventTime=NextLocalEventTime; */
             EventTime=nextTime;
	     if (GetNextMove(&theText,move) == 0)     /* Execute next command    */
	         execCmd(move);
	     else {
                if ((status==ACTIVE) && !(modeflag & AUTOKILL) ) {
                   sprintf(outs,"Frames %i Time %li ",frcount,thisTime);
                   Message(outs);
                   status=PAUSE;
                }
                if (modeflag & AUTOKILL) clean();
                   return 0;
             }
   } /* while */
   return 0;
}

void rMoveProc::execCmd(MOVEREC * move){
rMoveProc * child;
short n;
//long startOfMoveTime;
/*  Execute a command that has been parsed into a MOVEREC structure  */
//	startOfMoveTime=nextTime*100/localSpeed + startTime; /* lastTime is global time */
   switch ( move->cmdType ) {
      case IntCmd:
         switch (move->cmdNr) {
            case timecmd:
               /*LocalEventTime*/NextLocalEventTime=move->value;
               break;
            case deltaTimecmd:
               /*LocalEventTime*/NextLocalEventTime+=move->value;
               break;
            case setTimecmd:
               doSetClock(move->value);
               break;
            case starttimecmd:
               startTime=move->value;
               break;
            case tempocmd:
               doSetMetronome((int)(move->value));
               break;
            default: break;
         }
         nextTime=(100*NextLocalEventTime)/localSpeed+startTime;
         break;
      case VoidCmd:
         switch (move->cmdNr) {
            case lockposcmd:
            theBody->lockposflag=1;
            break;
            case unlockposcmd:
            theBody->lockposflag=0;
            break;
            default: break;
         }
         break;
      case StrCmd:        /*  Start a child MOVE process */
         child=spawn((MoveText *)(move->value),AUTOKILL);
         child->startTime=EventTime;
         child->activate();
         for (n=1; move[n].cmdNr != EndofBlock;n++) {
            if (move[n].cmdNr == duration) {
            child->localSpeed=(move[n].value>0)?(localSpeed*100)/move[n].value:10000;}
            if (move[n].cmdNr == mirrorcmd){
               child->readFlag ^= MIRROR;}
               }
            child->exec();
         break;
      case VHRBodyCmd:
      case XYZBodyCmd:
         theBody->SetThisTime(EventTime);
         if ( readFlag & MIRROR ) MirrorBodyMove(move);
         theBody->SetMove(move,EventTime,localSpeed);
         break;
      }

}



/* Create a subprocess of this MOVE text */
/* Used for MOVE functions               */
rMoveProc *  rMoveProc::spawn(MoveText * inbuf,int mode) {
rMoveProc * aProcess;
    aProcess=new(rMoveProc);
    /* Use inbuf  for same body starting at prsent time */
    aProcess->localSpeed=localSpeed;
    aProcess->parentSpeed=localSpeed;
    //aProcess->buffer=inbuf;
    aProcess->theText.setText(inbuf);
    //inbuf->incRef();
    aProcess->modeflag=mode;
    aProcess->theBody=theBody;
    aProcess->status=STOP;
/* Put new process last among rMoveProcesses, before theBody process list */
    ((RTKernel *)theKernel)->addProcess(aProcess,BEFORE,this->theBody);
    aProcess->readFlag=readFlag;
return aProcess;
}



void rMoveProc::clean(void){
/*    Message("Kill a rmov process");*/
    status = KILL;
//    if ((modeflag & DEALLOC) && buffer) free(buffer);
//    if (buffer) {
       //buffer->decRef();
       //buffer=0;
       theText.setText(NULL);
       //(buffer->active_count)--;
       //if ( (buffer->active_count <= 0) && (buffer->free_on_exit) ){
       //   free(buffer);
       //   buffer=0;
       //}
//    }
}

/* Cplusplus creator function */
int rMoveProc::init(void){
/* A set of sensible defaults */
   modeflag=0;
   status=STOP;
   //buffer=(MoveText *)0L;
   //theText.level=0;
   //theText.owner=(char *)this;
   startTime=nextTime=EventTime=thisTime;
   localSpeed=parentSpeed=100;
   readFlag=0;
   return 0;
}


rMoveProc::rMoveProc():theText((MoveText *)0L,this){
   //buffer((MoveText *)0L,this);
   init();
}


