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
*   Anima model robot joints
*
****************************************************************************/

extern long thisTime;

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
    char buf[64];
    sprintf((char *)buf, "(SETSERVOPORT - %s)",portname);
    Message(buf);
    ((mRCRobotProc *) bdy) -> setPortName(portname);
}

mRCRobotProc::mRCRobotProc(void) {
  installFun((char *)"SETSERVOPORT",VFuncR,(long)setServoPort);
  servoControl = NULL;
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
   double angle = 0;
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
      //printf("servo.moveto %i %i %i  time=%li \n",servonum[joint],RobAngle[joint],dt,thisTime);
      servoControl->moveto(servonum[joint], pos, dt);
   }
}

int mRCRobotProc::servovalue(int nr,double angle){
	SERVOPROP sp=servoProperties[nr];
	double v;
	int pos;
	v = sp.direction*angle*sp.resolution+sp.center;
	pos = (int)v;
	if (pos < sp.min)
		pos = sp.min;
	if (pos > sp.max)
		pos = sp.max;
	return pos;
};
