/* Real Time Kernel
Think C version  0.1  juli 91   Magnus Lundin
*/
#ifndef RTKERNEL_H
#define RTKERNEL_H

#include "rtproc.h"

enum { TOP,BOTTOM,BEFORE,AFTER };
typedef enum {KS_PAUSE, KS_READY, KS_BUSY } KSTATUS; /*  status for kernel */

class RTKernel {
public:
	RTKernel();
	void exec0(void);
	int exec(long arg);
/* Add a process p in a position relating to q */
	int addProcess(RTProcess * p,int mode,RTProcess * q);
	int removeProcess(RTProcess * p);
	int resetAllProc(void);

	KSTATUS	status;
	short   activity;

private:
	RTProcess * procList;
	RTProcess * curProcess;
};


#endif


