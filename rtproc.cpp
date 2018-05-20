#include "rtproc.h"
#include "kernel.h"
#include "clock.h"
//#include <stdio.h>
#ifdef THINK_C
#include <oops.h>
#endif

extern long thisTime;
/**********************************************************************************
The Process Class
A virtual class for processes executing under the kernel
*********************************************************************************/

RTProcess::RTProcess(void){
   priority=0;
   status=PAUSE;
   startTime=thisTime;
};

int RTProcess::activate(void) {
   status=ACTIVE;
   priority=0;
/* startTime=thisTime;
   nextTime=0;*/
   return 0;
}

int RTProcess::exec(void) {
   priority=0;
   return 0;
}

int RTProcess::reset(void) {
   return 0;
}

void RTProcess::clean(void){    /* change to clean !! */
//    HUnlock((Handle)this);
//printf("RTProces:clean( %li ): \n",this);
    status = KILL;
}


