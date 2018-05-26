#ifndef MRCROBOT_H
#define MRCROBOT_H
/*********************************************************************************
Real time kernel

Convert movements to RC Servo commands
*********************************************************************************/


#include <stdio.h>
#include "movelib.h"
#include "movetext.h"
#include "movebody.h"
#include "mbdyproc.h"
#include "ssc.h"

/*********************************************************************************
mRobotProc process class

*********************************************************************************/
typedef struct {
	char * namn;
	int min;
	int max;
	int center;
	int direction;
	float resolution /* steg/grad */;
//	char axis;
} SERVOPROP;

class mRCRobotProc : public  mBodyProc {
public:

    mRCRobotProc(void);
    ~mRCRobotProc();

    char filename[128];

//    int init(void);
    int reset(void);
    int exec(void);
    int activate(void);
    void clean(void);

    void SetMove(MOVEREC * move,long atTime,long localSpeed); // Overloaded
    virtual void setPortName(char * fn)=0;

protected:
    void  WriteRCMove();
    void  ComputeRobotAngles(long atTime);

    // Controller and servo dependent angle to value conversion
    virtual int servovalue(int nr, float angle);

	// Physical joint information
    int   nJoints;
    int * RobJoint;
    int * servonum;
    char * RobJointAx;

	// Servo control interface
    SERVOPROP * servoProperties;

	// Servo control interface
    rcServoController * servoControl;
};

#endif

