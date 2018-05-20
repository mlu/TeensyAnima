#ifndef MROBOT_H
#define MROBOT_H
/*********************************************************************************
Real time kernel

Convert movements to RAPID Commands
*********************************************************************************/
#ifdef Macintosh
#include <CList.h>
#include <CPane.h>
//#include "anisysd.h"
#endif

//#ifdef __MSDOS__
//#include "sys\anisysd.h"
//#endif

#include <stdio.h>
#include "movelib.h"
#include "movetext.h"
#include "movebody.h"
#include "mbdyproc.h"

/*********************************************************************************
mRobotProc process class

*********************************************************************************/
class mRobotProc : public  mBodyProc {
private:
#ifdef TCL_DECLARE_CLASS
TCL_DECLARE_CLASS
#endif
//    MOVEBODY	theBody;
//    long 	lastTime;

public:
//    int		dirty;          /* set if body moved */
//    int 	modelFlag;      /* set if shape should be calcolated */
//    int   	refnr;          /* used with aniglue to number the dancers in the system */
//    DANCER_ATTRIBS attribs;
//    int lockx,lockz,locknr,lockposflag;
//	 void  *      theShape;
    char filename[128];
    FILE * f;
	mRobotProc(void);

//    int init(void);
    int reset(void);
    int exec(void);
    int activate(void);
    //int newrMoveProc(MOVETEXT * inbuf,int mode);
    void clean(void);

//  Varible Acess Routines
    //void SetThisTime(long time);
    //void SetLastTime(long time);

    void SetMove(MOVEREC * move,long atTime,long localSpeed); // Overloaded
    void SetFileName(char * fn);

private:
    long cmdstart,cmdend,lastcmdend;
    void WriteRAPIDProgHead(void);
    void WriteRAPIDProgTail(void);
    void WriteRAPIDMove(char * tol);
    void ComputeRobotAngles(long atTime);
    long ComputeEndtime(long atTime);
};

#endif

