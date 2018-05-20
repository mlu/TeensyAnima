#ifndef MOLYMPIAROBOT_H
#define MOLYMPIAROBOT_H
/*********************************************************************************
Real time kernel

Convert movements to RC Servo commands
*********************************************************************************/


#include <stdio.h>
#include "movelib.h"
#include "movetext.h"
#include "movebody.h"
#include "mrcrobot.h"
#include "olympiaservo.h"

/*********************************************************************************
mRobotProc process class

*********************************************************************************/
class mOlympiaRobotProc : public mBodyProc {
public:

    mOlympiaRobotProc();
    ~mOlympiaRobotProc();
    const char * ClassName() { return "mOlympiaRobotProc";}
    char filename[128];

    int init(void);
    int reset(void);
    int exec(void);
    int activate(void);
    void clean(void);

    void SetMove(MOVEREC * move,long atTime,long localSpeed); // Overloaded
    void setPortName(char * fn);

private:
    void  ComputeRobotDistances(long atTime);
    long  FindNextFutureTime(long atTime);
    void  WriteRCMove();
    // Controller and servo dependent angle to value conversion
    virtual int servovalue(int nr,double angle);

    int   nJoints;
    int * RobJoint;
    int * servonum;
    char * RobJointAx;
    rcServoController * servo;
};

#endif

