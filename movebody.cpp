/*  Standard unit for MOVE ing a body
	Magnus Lundin, KACOR and CTH

	v3   Uses body angles in .1 degees fixpoint format.
	     change also  convVHR2Q and convQ2VHR  in translib.c
	     new version   translib2.c
*/
#include "movelib.h"
#include "movebody.h"
#include "movecmd.h"
#include "bdystruc.h"
#include <math.h>
#include <stdlib.h>  // Mac wants  IRIS dont like !!
#include <stdio.h>

#define ONE_TURN  3600

int ComputeBodyPos(int part,MOVEBODY * aBody,MOVE_POS *newPos) ;
int ComputeBodyPosVHR(int part,MOVEBODY * aBody, MOVE_POS * newPos);
int mod360(int a);
int dist360(int a, int b);
void normalizeVHR(VHR * angles);

/*********************************************************/

//#include "quicksin.h"
void SetBodyMove(MOVEBLOCK  theMove,MOVEBODY * aBody,long atTime, long localSpeed) {
      /* Updatera lastBodyPos till nulaeget */
      /* Om rorelsen avbryts skall nuvarande interpolerade pos, tid
	laeggas i lastBodyPos */
      /* Copy present position into nextpos */
      /* read all parameters to be changed  */
      /* Put present Time + Duration in nextBodyTime */
      /* nextBodyTime > presentTime signals an ongoing movement. */


      /* Movements are sent by an MOVEBLOCK ie an array:
        of MOVEREC : int cmdType, int cmdNr, long value
	*/

	int cmdtype,bodypart;
	MOVE_POS  newPos, lastPos, floorAngle;
	CInterpolationObject * aPart;
	int cmdnr;
	long value,dur;
	MOVEREC * paramp;
	int changed=0;
	unsigned long interpolationType;


	paramp=theMove;
	cmdtype=theMove->cmdType;
	bodypart=theMove->cmdNr;

	aPart=(aBody->jointData)+bodypart;
	aPart->getCurrentPos(CI_VHR,atTime,(POS_DATA *)&lastPos);
	dur=(10000/localSpeed);
	if (cmdtype==VHRBodyCmd) normalizeVHR(&lastPos);

	newPos=lastPos;               /* default new values are no change */

/************ IF body part is VHR joint **************************/
	if (cmdtype==VHRBodyCmd)
	while ( (cmdnr=(++paramp)->cmdNr) != EndofBlock) {
		paramp->cmdNr=NoToken;
		value=(paramp->value);
		switch (cmdnr) {
			case vertangle:
				newPos.V+=dist360(newPos.V,10*(int)value);
				changed=changed|V_SET;
				break;
			case rotangle:
				newPos.R+=dist360(newPos.R,10*(int)value);
				changed=changed|R_SET;
				break;
			case horangle:
				changed=changed|H_SET;
				newPos.H+=dist360(newPos.H,10*(int)value);
				break;
			case dvertangle:
				changed=changed|V_SET;
				newPos.V+=10*(int)value;
				break;
			case dhorangle:
				changed=changed|H_SET;
				newPos.H+=10*(int)value;
				break;
			case drotangle:
				newPos.R+=10*(int)value;
				changed=changed|R_SET;
				break;
			case duration:
				dur=(100*value/localSpeed);
				break;
			default :break;
		}
	}
/************ IF body part is XYZ position **************************/
	if ( cmdtype==XYZBodyCmd ) {

        /* Get stage direction */
        (aBody->jointData)[BODYTURN].getCurrentPos(CI_VHR,atTime,(POS_DATA *)&floorAngle);

		while ( (cmdnr=(++paramp)->cmdNr) != EndofBlock) {
			int value10;
			value=(paramp->value);
			value10=(int)(value*10);         /* convert cm to mm */
			switch (cmdnr) {
				case xpos:
					changed=changed|V_SET;
					newPos.V=value10;
					break;
				case ypos:
					changed=changed|H_SET;
					newPos.H=value10;
					break;
				case zpos:
					changed=changed|R_SET;
					newPos.R=value10;
					break;
				case dxpos:
					changed=changed|V_SET;
					newPos.V+=value10;
					break;
				case dypos:
					changed=changed|H_SET;
					newPos.H+=value10;
					break;
				case dzpos:
					changed=changed|R_SET;
					newPos.R+=value10;
					break;
				case rxpos:
					if (bodypart == BODYLOCATION) {
					changed=changed|V_SET|R_SET;
						newPos.V+=(short)(value10*cosf(3.1416*floorAngle.R/1800.0));
						newPos.R-=(short)(value10*sinf(3.1416*floorAngle.R/1800.0));
					}
					break;
				case rzpos:
					if (bodypart == BODYLOCATION) {
					changed=changed|V_SET|R_SET;
						newPos.R+=(short)(value10*cosf(3.1416*floorAngle.R/1800.0));
						newPos.V+=(short)(value10*sinf(3.1416*floorAngle.R/1800.0));
					}
					break;
				case duration:
					dur=(100*value/localSpeed);
					break;
				default :break;
			}
		}
	}


/********* Part Specific Rules ***************/
    switch (bodypart) {
	case RKNEE :
	case LKNEE :if (newPos.H > 900 + lastPos.H) {
									  newPos.H -= 1800; newPos.V = -newPos.V;}
				if (newPos.H < -900 + lastPos.H) {
									  newPos.H += 1800; newPos.V = -newPos.V;}
//				if (newPos.V > 0)   { newPos.V  = 0; printf("Illegal knee angle , time %li \n",aBody->thisTime);}
				break;
	case RELBOW:
	case LELBOW:if (newPos.H > 900)  { newPos.H -= 1800; newPos.V *= -1;}
				if (newPos.V < 0)   { newPos.V = 0;}
				break;
	default :	break;
	}

	if (cmdtype==VHRBodyCmd) interpolationType=CI_VHR;
	if (cmdtype==XYZBodyCmd) interpolationType=CI_XYZ;

	/*   Here we set up the interpolation mode and    ?check angles  */
	//if (aPart->flag>1) aPart->flag=1;            /*  all  joints has VHR as default */
	
	 if ((changed & V_SET) && (changed & H_SET))
	  if (abs(newPos.H - lastPos.H) >400 ) {
	    switch (bodypart) {
		case  PELVIS :
		case  WAIST :
		case  UPPERBACK :
		case  NECK :
		case  HEAD :
		case  RARM :
		case  RHIP :
		case  LARM :
		case  LHIP : 	interpolationType=CI_VH_R /*'QUAT' */;break;
		default :break;
		}
	   }
	aPart->setInterpolationData(changed,interpolationType,atTime,dur,&lastPos,&newPos);
	
	/*  JUMP LOGIC  - 95/03/09   */
	if (bodypart==BODYLOCATION) {
		if ( (lastPos.H==0)&&(newPos.H>0 )) aPart->setTimeAdjustment(H_SET | ACC_SET,0,0,-100);
		if ( (lastPos.H >0)&&(newPos.H==0)) aPart->setTimeAdjustment(H_SET | ACC_SET,0,0, 100);
	}
}


void MirrorBodyMove(MOVEBLOCK  theMove) {
      /* Movements are sent by an MOVEBLOCK ie an array:
        of MOVEREC : int cmdType, int cmdNr, long value  	*/

   int bodypart,mirror;
   int cmdnr;
   MOVEREC * paramp;
	paramp=theMove;
	bodypart=theMove->cmdNr;
        if ((bodypart >= 0) && (bodypart <= LASTPART)) {
            mirror=BodyStruct[bodypart].mirrorpart;
            if (mirror>=0) theMove->cmdNr=mirror;
	    while ( (cmdnr=(++paramp)->cmdNr) != EndofBlock) {
		switch (cmdnr) {
		case xpos:
		case dxpos:
		case rxpos:
		case rotangle:
		case horangle:
		case dhorangle:
		case drotangle:
			paramp->value= -(paramp->value);
			break;
		default :break;
		}
            }
        }
return;
}


/************************************************************/


void normalizeVHR(VHR * angles) {
/***       Normalize angular values   ****************/
   if (angles->V > ONE_TURN) angles->V  -= ONE_TURN;
   else
   if (angles->V < -ONE_TURN) angles->V += ONE_TURN;

   if (angles->H > ONE_TURN) angles->H  -= ONE_TURN;
   else
   if (angles->H < -ONE_TURN) angles->H += ONE_TURN;

   if (angles->R > ONE_TURN) angles->R  -= ONE_TURN;
   else
   if (angles->R < -ONE_TURN) angles->R += ONE_TURN;
}


/*  Distance modulo 360 , normalized to +- 180 */
int dist360(int a,int b) {
   int tmp;
   tmp=(b-a) % 3600;
   if (tmp > 1800) return (tmp-3600);
   if (tmp < -1800 ) return (tmp+3600);
   return tmp;
}

/*  Modulo 360, correct for negative arguments */
int mod360(int a) {
   int tmp;
   tmp=a % 3600;
   return (tmp < 0) ? tmp+3600 : tmp ;
};









