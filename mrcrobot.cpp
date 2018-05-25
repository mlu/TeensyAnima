/*@*********************************************************************************
mBodyProc process class

This is a MOVEBODY structure
*********************************************************************************/
#include  "mrcrobot.h"
#include  "clock.h"
#include <stdio.h>
#include <limits.h>

#include "movecmd.h"
#include "movetext.h"
#include "anicore.h"
//#include "anilink.h"
#include <typeinfo>

//#include <stdlib.h>
#include <string.h>
#include <errno.h>

/***************************************************************************
*
*   dirigent
*
****************************************************************************/

//typedef struct {char * namn;int min;int max;int center;int direction;double resolution /* steg/grad */;char axis;} SERVOPROP;

/*
SERVOPROP servoprop[8]={
   {"LARM V",20,230,20,1,1.17,'V'},
   {"LARM H",35,235,157,1,1.5,'H'},
   {"NONE",0,255,127,1,1.0,'H'},
   {"RARM V",30,230,230,-1,1.11,'V'},
   {"RARM R",50,190,116,-1,1.2,'R'},
   {"RELBOW V",50,190,180,-1,1.2,'V'},
   {"BODYTURN",25,230,127,1,1.4,'R'},
   {"HEAD R",40,220,127,1,1.5,'R'}
   };
*/
/***************************************************************************
*
*   Anima model robot joints
*
****************************************************************************/

//int nJoints=7;
int RobJoint[7]   ={ BODYTURN, RARM, RARM, RELBOW, LARM, LARM, HEAD};
int servonum[7]   ={ 6,        3,    4,    5,      0,    1,    7   };
char RobJointAx[7]={ 'R',      'V',  'R', 'V',     'V',  'H',  'R'};
int RobAngle[7];

extern long thisTime;
/* Cannot handle several servo controllers tied to different dancers */

//ssc * gservo=NULL;

//extern char outs[100];
extern int GetMoveString(MoveTextCursor * aText,MOVEREC * aMoveRec);
extern int GetMoveInteger(MoveTextCursor * aText,MOVEREC * move);

void setServoPort(MoveTextCursor * aText) {
int dnr;
char *portname;
short err;
MOVEREC aMoveRec;
    GetMoveString(aText,&aMoveRec);
    portname=(char *)(aMoveRec.value);
    mBodyProc * bdy=((rMoveProc *)(aText->getOwner()))->theBody;
    printf("(SETSERVOPORT - %s\n",portname);
    if (strcmp(bdy->ClassName(),"mMiniSscRobotProc")==0) {
       ((mRCRobotProc *) bdy)->setPortName(portname);
    }
    else if (strcmp(bdy->ClassName(),"mRCEzUSBProcProc")==0) {
       ((mRCRobotProc *) bdy)->setPortName(portname);
    }
}

mRCRobotProc::mRCRobotProc(void) {
  installFun((char *)"SETSERVOPORT",VFuncR,(long)setServoPort);
  servoControl = NULL;
  /*
  printf("Init:mRCRobotProc at %li   shape %li \n",this,theShape);
    {              
      FILE * f=fopen("servoconfig.txt","r");
      char inbuf[128];
      if (f) 
	{
	  fscanf(f,"%100s",inbuf);
	  this->setPortName(inbuf);
	  fclose(f);
	}
      else 
	{
	  char * errtxt =strerror(errno);
	  printf("Could not open servoconfig.txt - %s\n",errtxt);
	  setPortName("/dev/ttyS0");
	}
    }
  */
  reset();
}

mRCRobotProc::~mRCRobotProc() {
  rcServoController::stopupdate();
  printf( "smRCRobotProc remove dancer, port was %s \n",filename);
  if (servoControl != NULL ) {
     rcServoController::removeServo(servoControl);
     delete servoControl;
  }
}

int mRCRobotProc::activate(void) {
    reset();
    modelFlag=MODEL;
    status=ACTIVE;
    rcServoController::startupdate();
    return 0;
}

int mRCRobotProc::reset(void) {
    mBodyProc::reset();
#ifdef VERBOSE
printf("Init:mRCRobotProc at %li   shape %li \n",this,theShape);
#endif
return 0;
}

void mRCRobotProc::clean() {
    mBodyProc::clean();
    rcServoController::stopupdate();
    status = QUIT;
}


int mRCRobotProc::exec(void) {
    // SET SERVO POSITIONS !!
    mBodyProc::exec();
    WriteRCMove();
    return 0;
}


void mRCRobotProc::SetMove(MOVEREC * move,long atTime,long localSpeed){
   int mbreak=0,robotmove=0;
   // Filter out Non Robot Moves
   for (int i=0;i<nJoints;i++)
      if (RobJoint[i]==move->cmdNr) robotmove=1;
   // Set Movement command and Compute interpolated present position
   if (robotmove) SetBodyMove(move,&theBody,atTime,localSpeed);
}

void mRCRobotProc::WriteRCMove(){
   ComputeRobotAngles(thisTime);
   if (!rcServoController::updating()) {
      servoControl->updateservopos();
   }
}

void mRCRobotProc::ComputeRobotAngles(long atTime){
   /* If we are running:  */
   /* Compute position at next keyposition or maximum 500 ms in the future */
   /* else compute present position */
   /* Convert Anima VHR angles to servovalues */
   /* Send to servo as class as a ramp */
   int joint,pos,dt;
   double angle;
   long t,nt;
   MOVE_POS  Pos;
   for (joint=0;joint<nJoints;joint++) {
      if ( isRunning() ) {
         nt=theBody.jointData[RobJoint[joint]].nextKeyTime(atTime);
         if (nt<=atTime) continue;
         //printf("ComputeRobotAngles time=%li nt=%li %i\n",thisTime,nt,servonum[joint]);
         t=atTime+50;
         if (t>nt) t=nt;
      }
      else {
         t=atTime;
      }
      theBody.jointData[RobJoint[joint]].getCurrentPos(CI_VHR,t,(POS_DATA *)&Pos);
      switch (RobJointAx[joint]) {
         case 'R':
            angle=Pos.R/10;
         break;
         case 'V':
            angle=Pos.V/10;
         break;
         case 'H':
            angle=Pos.H/10;
         break;
      }
      /* Servo uses millisecon timing */
      dt=10*(t-atTime);
      pos = servovalue(servonum[joint],angle);
      RobAngle[joint]=pos;
      //printf("servo.moveto %i %i %i  time=%li \n",servonum[joint],RobAngle[joint],dt,thisTime);
      servoControl->moveto(servonum[joint],RobAngle[joint],dt);
   }
}
