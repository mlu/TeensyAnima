/* Real Time Kernel

Think C version  0.1

juli 91   Magnus Lundin
Modified for C++, september 91
*/
#ifndef RTPROC_H
#define RTPROC_H

#include "anisysd.h"

enum {QUIT,ACTIVE,WAIT,PAUSE,KILL,STOP,INACTIVE }; /*  status for processes */

#define DEALLOC  1
#define AUTOKILL 2

class RTKernel;

class RTProcess  {
protected:
public:
	RTKernel   * theKernel;
	RTProcess * nextProcess;
	short 	status;
	short 	priority;
	long	startTime;		/* base for local process time, ??set by RTK on start ??*/
	long	nextTime;		/* next sheduled execution time */
	int 	modeflag;
	RTProcess(void);
	virtual ~RTProcess(){};
    virtual const char * ClassName() { return "RTProcess";}
	virtual int  exec(void);
	//	virtual int  init(void);
	virtual int  activate(void);
	virtual int  reset(void);
	virtual void clean(void);
    virtual int setOption(const char * option,int value) { return -1;}
};

#endif

