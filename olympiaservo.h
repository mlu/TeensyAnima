#ifndef  _OLYMPIASERVO_H
#define  _OLYMPIASERVO_H

#include "rcservo.h"

#define olympiaservoBUFSIZE 48
#define OLYPIASERVOCMDLEN 8
#define OLYPIAINFOLEN 6
/***************************************************************************
*
*   class olympiaservo
*   Control of olympiaservo AVR8 servo controller
*
****************************************************************************/



class olympiaservo : rcServoController
{
public:
	olympiaservo();
	~olympiaservo(); 

	void openport(char * portname);
	void set(int nr,int pos);
	void send();

private:
#if defined(__WIN32__)
	HANDLE hComm;
#else
	int pfd;
#endif
	void sendCommand(unsigned char *cmd, int responselength);
	unsigned char buf[olympiaservoBUFSIZE];
	unsigned char bufs,bufe;
};


#endif
