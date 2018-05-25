#ifndef  _POLOLUSERVO_H
#define  _POLOLUSERVO_H

#include "rcservo.h"

/***************************************************************************
*
*   class pololuservo
*   Control of pololuC RC servo controller
*
****************************************************************************/

#define POLOLUBUFSIZE 48

class pololuservo : rcServoController
{
public:
	pololuservo();
	~pololuservo(); 
	
	void openport(char * portname);
	void set(int nr,int pos);
	void send();
	
private:
	long handle_error(long res);
	unsigned char buf[POLOLUBUFSIZE];
	unsigned char bufs,bufe;
};


#endif
