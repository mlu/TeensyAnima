/*@*********************************************************************************
  mBodyProc process class

  This is a MOVEBODY structure
*********************************************************************************/
#include  "mbdyproc.h"
#include  "rmovproc.h"
#include <stdio.h>

#include "movecmd.h"
//#include "anilink.h"

#ifdef THINK_C
#include <stdlib.h>
#include <oops.h>
//extern void Message(char * msg);
#else
//extern "C" void Message(char * msg);
//#include <malloc.h>
#include <stdlib.h>
#endif


extern long thisTime;
extern char outs[100];
int frcount;

extern MATERIAL_TYPE * getStandardMaterials(void);

#ifdef TCL_DECLARE_CLASS
TCL_DEFINE_CLASS_M0(mBodyProc);
#endif


mBodyProc::mBodyProc(void) {
  theShape=NULL;
  lockx=0;
  lockz=0;
  locknr=-1;
  lockposflag=0;
#ifdef USE_XDISPLAY
  attribs.material=getStandardMaterials();    /* In sysdep drawroutine */
#endif
  reset();
}

mBodyProc::~mBodyProc() {
#ifdef USE_XDISPLAY
  free(attribs.material);
#endif
}

int mBodyProc::activate(void) {
  reset();
  modelFlag=MODEL;
  status=ACTIVE;
  return 0;
}

int mBodyProc::reset(void) {
  int part;
  //HLock((Handle)this);
  lastTime=thisTime;
  //  InitMovebody(&theBody);
  theBody.thisTime=0;
  for (part=0;part<LASTPART;part++) {
    theBody.jointData[part].reset();
    if ((part==BODYLOCATION) || (part==PELVISLOC)) theBody.jointData[part].setInterpolationType(CI_XYZ);
    else theBody.jointData[part].setInterpolationType(CI_VHR);
  }

  for (part=0;part<LASTPART;part++) theBody.jointData[part].getCurrentPos(CI_QUAT,theBody.thisTime,theBody.jointPosQ+part);
  ModelBody_Q(theBody.jointPosQ,&theShape,&lockx,&lockz,&locknr);
#ifdef VERBOSE
  printf("Init:mBodyProc at %li   shape %li \n",this,theShape);
#endif
  modelFlag=MODEL;
  status=ACTIVE;
  frcount=0;
  lastTime=thisTime;
  //HUnlock((Handle)this);
  return 0;
}

extern int gLockPosFlag;

int mBodyProc::exec(void) {
  int part,m,mov=0,newstat=PAUSE;
  lastTime=thisTime;
  theBody.thisTime=thisTime;
  gLockPosFlag=lockposflag;
  //  Compute current position with interpolation
  for (part=0;part<LASTPART;part++) {
    m=theBody.jointData[part].getCurrentPos(CI_QUAT,theBody.thisTime,theBody.jointPosQ+part);
    mov=mov||m;
  }
  if (mov) newstat=ACTIVE;
  if ( (status == ACTIVE)|( newstat == ACTIVE) )
    ModelBody_Q(theBody.jointPosQ,&theShape,&lockx,&lockz,&locknr);

#ifdef REMOTE_ANILINK_KERNEL
  if (refnr==0) AnimaLinkSend(AL_BODY_SHAPE, refnr ,sizeof(BODYSHAPE),(char *)theShape);
#endif

  frcount++;
  dirty=0;
  status=ACTIVE; //newstat;
  return 0;
}

/* Create a  MOVE text for this body */
/* Used for text command window      */
int mBodyProc::newrMoveProc(MoveText * inbuf,int mode) {
  rMoveProc * aProcess;
  aProcess=new(rMoveProc);
  aProcess->init();
  /* Use inbuf  for same body starting at present time */
  aProcess->theText.setText(inbuf);
  aProcess->theBody=this;
  aProcess->modeflag=mode;
  /* Put new process BEFORE this in process list */
  ((RTKernel *)theKernel)->addProcess(aProcess,BEFORE,this);
  return 0;
}


void mBodyProc::clean() {
  //    HUnlock((Handle)this);
  if (theShape != NULL) {
      free(theShape);
      theShape = NULL;
  }
  status = QUIT;
}


void mBodyProc::SetThisTime(long time) {
  theBody.thisTime=time;
}

void mBodyProc::SetLastTime(long time) {
  lastTime=time;
}

void mBodyProc::SetMove(MOVEREC * move,long atTime,long localSpeed){
  SetBodyMove(move,&theBody,atTime,localSpeed);
}






