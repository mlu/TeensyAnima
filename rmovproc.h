#ifndef MOVEPROC_H
#define MOVEPROC_H
/*********************************************************************************
Real time kernel

Processes for MOVE
*********************************************************************************/
#include "kernel.h"
#include "mbdyproc.h"
#include "movelib.h"
#include "movetext.h"

#define MIRROR    1


/*********************************************************************************
rMOVE process class

a MOVE text file input process
*********************************************************************************/
/** @todo  local symbols
*   Every rmoveproc has a symbol table that links to the parent process symbol
*   table or the global symbol table
*   HOW do we control local/global variable acces ????????????
*/

/** @todo  loops, .. ..
*   Loop counter, this is rather specific ??!!
*   While (condition)
*/

class rMoveProc : public RTProcess {
public:
#ifdef TCL_DECLARE_CLASS
TCL_DECLARE_CLASS
#endif
    MoveTextCursor theText;    /* Used by command parser  */
    //MoveText * buffer;       /* Pointer to the text buffer,  different if not MoveText process */
    mBodyProc *  theBody;      /* body to send movement commands to */
    long 	localSpeed;    /* Local process speed in procent */
    long 	parentSpeed;   /*  Parent process speed   */
    long 	EventTime,/*LocalEventTime,*/NextLocalEventTime;
    int     readFlag;
    rMoveProc();
    int init(void);
    int reset(void);
    int exec(void);
    int activate(void);
    rMoveProc *  spawn(MoveText * inbuf,int mode);
    void clean(void);
    void execCmd(MOVEREC * move);
};
#endif

