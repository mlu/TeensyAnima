/***********************************************************************************
CInterpo.cpp
class CInterpolationObject

A class for interpolation objects,
Indata is alvays in the form of three integers (V,H,R) a time and a duration.
The interpolation can be linear in the three variables separately  or linear
quaternion interpolation.
Outdata can be of the form V,H,R or as quatrnions independently of the
interpolation used.

95-02-27  Magnus Lundin
97-03-19
Added time sorting when setting up interpolation data
test of the form
    if (time >= StartTime[xx] ) {
    .. doit
    }; // ELSE DONT CHANGE THIS PART

************************************************************************************/

#include <math.h>
//#include <values.h>
#include "cinterpo.h"
#include "translib.h"
/**************************       IMPLEMENTATION        *****************************/
MOVE_POS NULLPOS={0,0,0};
Quat     NULLQUAT={1.0,0.0,0.0,0.0};

CInterpolationObject::CInterpolationObject(void) {reset();}

void CInterpolationObject::reset(void)
{
	int i;
	prevVHR=nextVHR=NULLPOS;
	prevQ=dlogQ=NULLQUAT;

	for (i=0;i<4;i++) {
		startTime[i]=endTime[i]=0;
		acceleration[i]=0;
		easein[i]=easeout[i]=0;
	}
}

void   CInterpolationObject::setInterpolationType(unsigned long newtype){
	type=newtype;
	if (type==CI_QUAT) setupQuatInterp();
}

void   CInterpolationObject::setInterpolationData(
short part,
unsigned long newtype,
long time,
long dur,
MOVE_POS * startPos,MOVE_POS * endPos)
{
	int i;
	type=newtype;
	for (i=0;i<4;i++) {
		acceleration[i]=0;
		easein[i]=easeout[i]=0;
	}
	if (type==CI_QUAT) {
		if (time >= startTime[0] ){
			startTime[0]=time;
			endTime[0]=time+dur;
			prevVHR=*startPos;
			nextVHR=*endPos;
			setupQuatInterp();
			return;
		}
	}
	if (part & R_SET) {
		if (time >= startTime[2] ) {
			prevVHR.R=startPos->R;
			nextVHR.R=endPos->R;
			startTime[2]=time;
			endTime[2]=time+dur;
		}
	}
	if (type==CI_VH_R) {
		if (time >= startTime[0] ) {
			if (part & (V_SET|H_SET)) {
				prevVHR.V=startPos->V;
				nextVHR.V=endPos->V;
				prevVHR.H=startPos->H;
				nextVHR.H=endPos->H;
				startTime[0]=startTime[1]=time;
				endTime[0]=endTime[1]=time+dur;
			}
			setupVH_RInterp();
			return;
		}
	}
	if (part & V_SET) {
		if (time >= startTime[0] ){
			prevVHR.V=startPos->V;
			nextVHR.V=endPos->V;
			startTime[0]=time;
			endTime[0]=time+dur;
		}
	}
	if (part & H_SET) {
		if (time >= startTime[1] ){
			prevVHR.H=startPos->H;
			nextVHR.H=endPos->H;
			startTime[1]=time;
			endTime[1]=time+dur;
		}
	}
//startTime[3]=time;
//endTime[3]=time+dur;
}

/**********************************************************************************
void   CInterpolationObject::setTimeAdjustment(short param,short ei,short eo,short acc);

**********************************************************************************/

void   CInterpolationObject::setTimeAdjustment(short param,short ei,short eo,short acc)
{
	if (param & ACC_SET) {
		if (param & V_SET) acceleration[0]=acc;
		if (param & H_SET) acceleration[1]=acc;
		if (param & R_SET) acceleration[2]=acc;
	}

	if (param & EIO_SET) {
		if (param & V_SET) { easein[0]=ei;easeout[0]=eo;}
		if (param & H_SET) { easein[1]=ei;easeout[1]=eo;}
		if (param & R_SET) { easein[2]=ei;easeout[2]=eo;}
	}
}
/**********************************************************************************
void   CInterpolationObject::setstartTime(long time)

Move the beginning of the interpolation interval to time.

Who shall know about these things, CInterpolationObject or the object owners ?
What happens with a different interpolation model ?
**********************************************************************************/
void   CInterpolationObject::setStartTime(long time){
POS_DATA tempPos;
int i;
	getCurrentPos(CI_VHR,time,&tempPos);
	for (i=0;i<4;i++) startTime[i]=time;
	prevVHR=tempPos.p;
	if (type==CI_QUAT)  setupQuatInterp();
}

/**********************************************************************************
short  CInterpolationObject::getCurrentPos(
unsigned long restype,
long atTime,
POS_DATA * pos)
**********************************************************************************/

short  CInterpolationObject::getCurrentPos(
unsigned long restype,
long atTime,
POS_DATA * pos)
{
POS_DATA  tempPos;
//if (startTime==atTime) switch (restype) {
//	case CI_QUAT: pos->q=lastQ;return 1;break;
//	case CI_VHR: pos->p=lastVHR;return 1;break;
//		}
switch (restype) {
	case CI_VHR:
		switch (type) {
			case CI_QUAT:
				compQfromQ(&tempPos.q,atTime);
				convQ2VHR((MOVE_POS *)pos, &tempPos.q );
				break;
			case CI_VH_R:
				compVH_RfromVH_R((MOVE_POS *)pos,atTime);
				break;
			case CI_XYZ:
			case CI_VHR:
				compVHRfromVHR((MOVE_POS *)pos,atTime);
				break;
			default:pos->p=NULLPOS;
		}      /* switch (type) */
		break; /* restype CI_VHR  */
	case CI_QUAT:
		switch (type) {
			case CI_XYZ:
				compVHRfromVHR((MOVE_POS *)pos,atTime);
				break;
			case CI_VH_R:
				compVH_RfromVH_R((MOVE_POS *)pos,atTime);
				convVHR2Q((Quat *)pos ,(MOVE_POS *)pos );
				break;
			case CI_VHR:
				compVHRfromVHR((MOVE_POS *)pos,atTime);
				convVHR2Q((Quat *)pos ,(MOVE_POS *)pos );
				break;
			case CI_QUAT:
				compQfromQ((Quat *)pos ,atTime);
				break;
			default:pos->q=NULLQUAT;
		}    /* switch (type) */
		break;  /* restype CI_QUAT  */
	}	/*  switch (restype)  */
	return 1;    /*  1 signal's an ongoing movement */
}

long   CInterpolationObject::nextKeyTime(long time){
	long t=MAXLONG;
	if ((endTime[0]>time)&&(endTime[0]<t)) t=endTime[0];
	if ((endTime[1]>time)&&(endTime[1]<t)) t=endTime[1];
	if ((endTime[2]>time)&&(endTime[2]<t)) t=endTime[2];
	if (t<MAXLONG) {
		return t;
	} else {
		return -1;
	}
}



short  CInterpolationObject::isKeyTime(long time0,long time1){
	return 0;
}

/**************************       PRIVATE FUNCTIONS         *****************************/
anifloat smooth_time(int ease_in,int ease_out,anifloat t);
anifloat normalize_time(long start_time,long end_time,long time);
anifloat skew_time(int acc,anifloat t);
int gSmoothTime=100;

/* This should perhaps be inline, code or a macro */
inline anifloat normalize_time(long start_time,long end_time,long time)
{
	return ((anifloat)(time-start_time))/((anifloat)(end_time-start_time));
}

inline anifloat CInterpolationObject::adjust_time(short n,anifloat t)
{
	if (gSmoothTime==0) return t;
	if (acceleration[n]) return skew_time(acceleration[n],t);
	if (easein[n] || easeout[n]) return smooth_time(easein[n],easeout[n],t);
	return smooth_time(gSmoothTime,gSmoothTime,t);
}

int CInterpolationObject::compQfromQ(Quat * destQ ,long atTime ) {
	anifloat t;
	Quat dQ;
	if ( (endTime[3]-atTime) > 0 ) {
		if (  (atTime-startTime[3])> 0 ) {
			t=normalize_time(startTime[3],endTime[3],atTime);//((anifloat)t1)/(t1+t2);
			t=adjust_time(0,t);
			mulQuatFloat(& dQ, &dlogQ, t);
			expQuat(& dQ, & dQ);
			mulQuatQuat(destQ, &dQ, &prevQ );
			flag=2;
			return 1;
		}
		else  /* if (t1 <= 0 ) WE ARE NOT STARTED  */
		{
		   *destQ=prevQ;
		   flag=2;
		   return 1;
		}
	}
	else     /* if (t2 <= 0 )  WE HAVE FINISHED  */
	{
	//   if (flag == 2) {
		expQuat(& dQ, &(dlogQ));
		mulQuatQuat(destQ, & dQ, & (prevQ) );
		flag=3;
		return 0;
	};
}

int CInterpolationObject::compVH_RfromVH_R(VHR * destVHR , long atTime) {
	int r=0;
	anifloat t,cv,sv;
	MOVE_POS newPos;
	Point3f pos;

	if ( (endTime[2]-atTime) > 0 ) {
		if (  (atTime-startTime[2])> 0 ) {
			t=normalize_time(startTime[2],endTime[2],atTime);
			t=adjust_time(2,t);
			newPos.R=(short)(((1-t)*prevVHR.R+t*nextVHR.R));
		    r=1;
	   	}
		else  newPos.R = prevVHR.R ; /* if (t1 <= 0 ) */
	}
	else  newPos.R = nextVHR.R ;    /* if (t2 <= 0 ) */

	if ( (endTime[0]-atTime) > 0 ) {
		if (  (atTime-startTime[0])> 0 ) {
			t=normalize_time(startTime[0],endTime[0],atTime);//((anifloat)t1)/(t1+t2);
			t=adjust_time(0,t);
			cv=cosf(t*dlogQ.w);
			sv=sinf(t*dlogQ.w);
			pos.x=cv*prevQ.x+sv*dlogQ.x;
			pos.y=cv*prevQ.y+sv*dlogQ.y;
			pos.z=cv*prevQ.z+sv*dlogQ.z;
			convPoint2VH(&newPos,&pos);
		}
		else  {
			newPos.V = prevVHR.V ; /* if (t1 <= 0 ) */
			newPos.H = prevVHR.H ;
		}
	}
	else  { 
		newPos.V = nextVHR.V ;    /* if (t2 <= 0 ) */
	    newPos.H = nextVHR.H ; 
	}
	*destVHR=newPos;
	return r;
}

int CInterpolationObject::compVHRfromVHR(VHR * destVHR , long atTime)
{
	int r=0;
	anifloat t;
	MOVE_POS newPos;
	/* Interpolate to present time */
	if ( (endTime[0]-atTime) > 0 )
	   if (  (atTime-startTime[0])> 0 ) {
		t=normalize_time(startTime[0],endTime[0],atTime);
		t=adjust_time(0,t);
	      newPos.V=(short)(((1-t)*prevVHR.V+t*nextVHR.V));
	      r=1;
	   	}
	   else  newPos.V = prevVHR.V ; /* if (t1 <= 0 ) */
	else  newPos.V = nextVHR.V ;    /* if (t2 <= 0 ) */
	if ( (endTime[1]-atTime) > 0 )
	   if (  (atTime-startTime[1])> 0 ) {
		t=normalize_time(startTime[1],endTime[1],atTime);
		t=adjust_time(1,t);
	      newPos.H=(short)(((1-t)*prevVHR.H+t*nextVHR.H));
	      r=1;
	     }
	   else
	   newPos.H = prevVHR.H ;        /* if (t1 <= 0 ) */
	else   newPos.H = nextVHR.H ;    /* if (t2 <= 0 ) */
	if ( (endTime[2]-atTime) > 0 )
	   if (  (atTime-startTime[2])> 0 ) {
		  t=normalize_time(startTime[2],endTime[2],atTime);
		  t=adjust_time(2,t);
	      newPos.R=(short)(((1-t)*prevVHR.R+t*nextVHR.R));
	      r=1;
	   	}
	   else  newPos.R = prevVHR.R ; /* if (t1 <= 0 ) */
	else  newPos.R = nextVHR.R ;    /* if (t2 <= 0 ) */
	*destVHR=newPos;
	return r;
}

int CInterpolationObject::setupQuatInterp(void) {
Quat tQ;
	convVHR2Q(&prevQ , &prevVHR );
	convVHR2Q( &tQ , &nextVHR );
    divQuatQuat( &tQ, &tQ, &prevQ );
    if (tQ.w < 0) {tQ.w= -tQ.w;tQ.x= -tQ.x;tQ.y= -tQ.y;tQ.z= -tQ.z;} /* Use short way */
    logQuat( &dlogQ , &tQ );
    flag=2;
return 0;
}

int CInterpolationObject::setupVH_RInterp(void) {
Quat tQ;
anifloat theta,costheta,norm;
	convVH2Point((Point3f *)&prevQ.x , &prevVHR );
	convVH2Point((Point3f *)&tQ.x , &nextVHR );
	costheta=tQ.x*prevQ.x+tQ.y*prevQ.y+tQ.z*prevQ.z;
	theta=acos(costheta);
	tQ.x-=prevQ.x*costheta;
	tQ.y-=prevQ.y*costheta;
	tQ.z-=prevQ.z*costheta;
	norm=sqrt(tQ.x*tQ.x+tQ.y*tQ.y+tQ.z*tQ.z);
	if (norm<0.001) {
		if (prevQ.z>0) {
			tQ.x=0.0;tQ.y=-prevQ.z;tQ.z=prevQ.y;
		}
		else {
			tQ.x=0.0;tQ.y=prevQ.z;tQ.z=-prevQ.y;
		};
	};
	tQ.x=tQ.x/norm;
	tQ.y=tQ.y/norm;
	tQ.z=tQ.z/norm;
	tQ.w=theta;
    dlogQ=tQ;
return 0;
}

/*  Cubic Bezier interpolation of normalized times */

anifloat smooth_time(int ease_in,int ease_out,anifloat t)
{
anifloat e1,e2;
anifloat s0,s1,s2;
	e1=((anifloat)(100-ease_in))/300.0;
	e2=1.0-((anifloat)(100-ease_in))/300.0;
	s0=t*e1;s1=e1+t*(e2-e1);s2=e2+t*(1-e2);
	s0=s0+t*(s1-s0);s1=s1+t*(s2-s1);
	s0=s0+t*(s1-s0);
return s0;
}

/*  Quadratic Bezier interpolation of normalized times */
/*  acc=100  accelerates velocity linearly from 0              */
/*  acc=-100 retards velocity linearly to 0                    */

anifloat skew_time(int acc,anifloat t)
{
anifloat e;
anifloat s0,s1;
	e=((anifloat)(100-acc))/200.0;
	s0=t*e;s1=e+t*(1-e);
	s0=s0+t*(s1-s0);
return s0;
}



