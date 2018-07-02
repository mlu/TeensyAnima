/* Real Time Kernel

Think C version  0.1

juli 91   Magnus Lundin
*/

#include "movelib.h"
#include "kernel.h"
#include "clock.h"
#include <stdio.h>

extern int running;

/*********************************************************************************
The Kernel code

*********************************************************************************/

RTKernel::RTKernel(void) {
//Message("Kernel creator");
	procList=new(RTProcess);  /* create a list head */
	procList->nextProcess=0L;
	status=KS_READY;
	thisTime=0;
}

int RTKernel::exec(long arg) {
RTProcess * prevProcess;
if (status==KS_BUSY) return 0;  // Prevent reentry

while ((arg-- >= 0) ) {
	if (! procList->nextProcess)  {
		status=KS_READY;
		return 0; 
	}
	status=KS_BUSY;
		
	doClock();
	/*  Run all due processes  */
	activity=0;
	
	// Repeat until no next process
	for (prevProcess=procList;prevProcess->nextProcess;) {
		short pstat;
		curProcess=prevProcess->nextProcess;
		pstat=curProcess->status;
		activity=activity || (pstat == ACTIVE);  // Check if any process is currently active
		switch (pstat) {
			case PAUSE:
			case ACTIVE:
				if ( (curProcess->priority)>=0) {
					curProcess->exec();
				}
				else {
					++(curProcess->priority);
				}
				break;
			
			case WAIT:
				if ( (curProcess->nextTime)<=thisTime) {
					curProcess->exec();
				};
				break;
				
			case QUIT:        /* Remove a process  */
			case KILL:        /* Delete and Remove a process  */
				prevProcess->nextProcess=curProcess->nextProcess;
				#ifdef VERBOSE
				printf("Removed a kill process %li \n",curProcess);
				#endif
				delete(curProcess);
				curProcess=prevProcess;
				break;

			case INACTIVE:
			default:
				//		printf("Default in kernel exec at %li \n",curProcess);
				break;
			}
			prevProcess=curProcess;
		};

	}

	status=KS_READY;
	return activity; // && running;
}

int RTKernel::addProcess(RTProcess * p,int mode,RTProcess * q) {
RTProcess * temp;
RTProcess * next;
//printf("add process %li \n",p);
switch (mode) {
	case BOTTOM:	/* Find last process */
		for (temp=procList;temp->nextProcess;temp=temp->nextProcess) {};
		break;
	case BEFORE:
		for (temp=procList;(temp->nextProcess)&&(temp->nextProcess!= q)
			;temp=temp->nextProcess) {};
		break;
	case AFTER:
		for (temp=procList;(temp->nextProcess)&&(temp != q)
			;temp=temp->nextProcess) {};
		break;
	case TOP:
	default:
		temp=procList;
	}
	/* Now isert the new process */
	next=temp->nextProcess;
	temp->nextProcess=p;
	p->nextProcess=next;
	/* Do some initializing on the new process */
	p->activate();
	p->priority=0;
	p->theKernel=this;
	return 0;
}

int RTKernel::removeProcess(RTProcess * p) {
RTProcess * temp;

for (temp=procList;(temp->nextProcess)&&(temp->nextProcess!= p)
			;temp=temp->nextProcess) {};
if (temp->nextProcess) {
	/* Now remove the process */
	temp->nextProcess=p->nextProcess;
	p->nextProcess=0L;
	p->status=QUIT;
	p->theKernel=0L;
	return 1;
	}
else return 0;     /* Not found */

}



int RTKernel::resetAllProc(void) {
RTProcess * temp;
	if (! procList->nextProcess) { return 0; };
        status=KS_BUSY;
	/* init the clock */
	/*  doStartClock(); */
	/*  Reset all due processes  */
	for (temp=procList;temp->nextProcess;temp=curProcess) {
		curProcess=temp->nextProcess;
                if (curProcess == 0L ) {
                     status=KS_READY;
                     return 0;
		}
		if ((curProcess->status != QUIT) && (curProcess->status != KILL)) {
		   curProcess->status=ACTIVE;
		   curProcess->reset();
		}
	};
        status=KS_READY;
	return 0;
}

void RTKernel::exec0() {
	exec(0);
}


