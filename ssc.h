#ifndef  _SSH_H
#define  _SSH_H

#include "rcservo.h"

#define SSCBUFSIZE 48

/***************************************************************************
*
*   class ssc
*   Control of miniSSC RC servo controller
*
****************************************************************************/



class ssc : rcServoController
{
 public:
  ssc();
  ~ssc(); 

  void openport(char * portname);
  void set(int nr,int pos);
  void send();
 private:
#if defined(__WIN32__)
	HANDLE hComm;
#else
	int pfd;
#endif
  unsigned char buf[SSCBUFSIZE];
  unsigned char bufs,bufe;
};


#endif
