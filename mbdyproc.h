#ifndef MBODY_H
#define MBODY_H
/*********************************************************************************
Real time kernel

Processes for MOVE
*********************************************************************************/
#ifdef Macintosh
#include <CList.h>
#include <CPane.h>
//#include "anisysd.h"
#endif

//#ifdef __MSDOS__
//#include "sys\anisysd.h"
//#endif

#include "movelib.h"
#include "movetext.h"
#include "movebody.h"
#include "kernel.h"

/* Bit constants for modelFlag */
#define   MODEL     1

/*********************************************************************************
bodyProc process class

This is a MOVEBODY structure
*********************************************************************************/
class mBodyProc : public  RTProcess {
#ifdef TCL_DECLARE_CLASS
TCL_DECLARE_CLASS
#endif
protected:
public:
    MOVEBODY	theBody;
    long 	lastTime;

    int		dirty;          /* set if body moved */
    int 	modelFlag;      /* set if shape should be calcolated */
    int   	refnr;          /* used with aniglue to number the dancers in the system */
    DANCER_ATTRIBS attribs;
    anifloat      lockx,lockz;
    int         locknr,lockposflag;
    Point3 *    theShape;


    mBodyProc(void);
    ~mBodyProc();

    int reset(void);
    int exec(void);
    int activate(void);
    int newrMoveProc(MoveText * inbuf,int mode);
    void clean(void);

//  Varible Acess Routines
    void SetThisTime(long time);
    void SetLastTime(long time);

    virtual void SetMove(MOVEREC * move,long atTime,long localSpeed);

};

#endif

