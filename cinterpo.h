/***********************************************************************************
CInterpo.h

class CInterpolationObject

A class for interpolation objects,
Indata is alvays in the form of three integers (V,H,R) a time and a duration.
The interpolation can be linear in the three variables separately  or linear
quaternion interpolation.
Outdata can be of the form V,H,R or as quatrnions independently of the
interpolation used.

95-02-27  Magnus Lundin
************************************************************************************/

#include "anitypes.h"
#ifndef MAXLONG
#define MAXLONG  0x7FFFFFFF
#endif

enum {CI_VHR,CI_XYZ,CI_QUAT,CI_VH_R};
/*
Interpolation modes:
CI_VHR: Uses separate time, easein/out and acceleration channels for V, H and R respectively
CI_XYZ is similar to CI_VHR

CI_VH_R: Uses on twodimensional interpolation with time and ei/o for VH and separate onedimensional 
interpolation ofr R channel.

CI_QUAT: One threedimensional intepolation for VHR together
*/
#define V_SET	1
#define H_SET	2
#define R_SET	4
#define X_SET	1
#define Y_SET	2
#define Z_SET	4
#define EIO_SET 8
#define ACC_SET 16

#define V_EIO	1
#define X_EIO	1
#define H_EIO	2
#define Y_EIO	2
#define R_EIO	4
#define Z_EIO	4
#define Q_EIO	8
#define V_ACC	16
#define X_ACC	16
#define H_ACC	32
#define Y_ACC	32
#define R_ACC	64
#define Z_ACC	64
#define Q_ACC	128

class CInterpolationObject {
public:
   CInterpolationObject(void);
   void   reset(void);

   void   setInterpolationType(unsigned long type);
   void   setInterpolationData(short pram,unsigned long type,long time,long dur,MOVE_POS * startPos,MOVE_POS * endPos);
   void   setTimeAdjustment(short param,short ei,short eo,short acc);
   void   setStartTime(long time);
   short  getCurrentPos(unsigned long type,long time,POS_DATA * pos);

   long   nextKeyTime(long time);
   short  isKeyTime(long time0,long time1);
private:
   MOVE_POS	prevVHR;
   MOVE_POS	nextVHR;
   long		startTime[4];
   long		endTime[4];
   Quat		prevQ;
   Quat		dlogQ;
   short	easein[4];
   short	easeout[4];
   short	acceleration[4];
   short	time_ctrl;
   short	flag;
   long		type;


   int compQfromQ(Quat * destQ ,long atTime );
   int compVHRfromVHR(VHR * destVHR , long atTime);
   int compVH_RfromVH_R(VHR * destVHR , long atTime);
   int setupQuatInterp(void);
   int setupVH_RInterp(void);
   anifloat adjust_time(short n,anifloat t);

};

