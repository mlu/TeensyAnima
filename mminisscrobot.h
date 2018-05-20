#ifndef MMINISSCROBOT_H
#define MMINISSCROBOT_H
/*********************************************************************************
Real time kernel

Convert movements to RC Servo commands
*********************************************************************************/


#include <stdio.h>
#include "movelib.h"
#include "movetext.h"
#include "movebody.h"
#include "mrcrobot.h"
#include "ssc.h"

/*********************************************************************************
mRobotProc process class

*********************************************************************************/
class mMiniSscRobotProc : public mRCRobotProc {
public:

    mMiniSscRobotProc();
    //    ~mMiniSscRobotProc();
    const char * ClassName() { return "mMiniSscRobotProc";}
//    char filename[128];

//    int init(void);
//    int reset(void);
//    int activate(void);
//    void clean(void);

    //    void SetMove(MOVEREC * move,long atTime,long localSpeed); // Overloaded
    void setPortName(char * fn);

private:
    int servovalue(int nr,double angle);
    //ssc * servo;
};

#endif

