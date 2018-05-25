#ifndef MPETRUSJKA_H
#define MPETRUSJKA_H

#include "mrcrobot.h"

#include "pololuservo.h"

/* Hall sensor pulse counter for Teensy 3.6 */
extern uint32_t PulseCount();

class mPetrusjkaProc : public  mRCRobotProc {
public:
    mPetrusjkaProc(void);
    const char * ClassName() { return "mPetrusjkaProc";}
    void setPortName(char * fn);
    int exec(void);
    int setOption(const char * option,int value); 

private:
    int servovalue(int nr,double angle);
    int pulsecontrol;
    unsigned int lastpulsecount;
    double activitylevel;
};

#endif
