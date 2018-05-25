#ifndef  _RCSERVO_H
#define  _RCSERVO_H

//#include "asystype.h"
#include "anitypes.h"
#include "movelib.h"

typedef struct {int position;int target;int intervall;bool active;bool inupdate;} servoctrl;
typedef struct {long lasttime; servoctrl servo[16];} servoblock;

/***************************************************************************
*
*   class ssc
*   Control of miniSSC RC servo controller
*
****************************************************************************/


class rcServoController
{
public:
  rcServoController();
  virtual ~rcServoController(); 

  static void addServo(rcServoController * servo);
  static void removeServo(rcServoController * servo);
  static void startupdate();
  static void stopupdate();
  static const bool updating() {return isupdating;} 

  void moveto(int nr, int pos,int intervall);
  void updateservopos();
  
  virtual void openport(char * portname)=0;
  /* Set devicedependent command  value in output buffer */
  virtual void set(int nr,int pos)=0;
  virtual void send()=0;

protected:
  int servoblocksize;
  servoblock sb;
  bool isopen;
  static bool isupdating;
};

#endif
