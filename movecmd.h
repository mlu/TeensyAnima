/*    movecmd.h   ---   Header file to be used with MOVELIB toolbox */

#ifndef _MOVECMD_H_
#define _MOVECMD_H_ 1

//#include "movetext.h"
class MoveTextCursor;
class rMoveProc;

enum TOKENTYPE {NoToken=-100,
                leftP,rightP,
                endBuffer,
                IntToken,EndofBlock=0,
                VHRBodyCmd,XYZBodyCmd,
IntCmd,VoidCmd,NameParam,CameraCmd,Comment,StrCmd,SysCmd,NewSymbol,ProcFlag,
VFuncS,                 //   A callable void func with 1 string arg.
VFuncR,                 //   A callable void func with 1 MOVEREADPOS    pointer arg.
VFuncI,                 //   A callable void func with 1 int arg.      (long int !!)
IFuncI,                 //   A callable int func with 1 int arg.      (long int !!)
IFuncII,                //   A callable int func with 2 int arg.      (long int !!)
Other};

#define TOKENNAMESIZE  31
typedef struct  {char name[TOKENNAMESIZE+1] ;enum TOKENTYPE type;long value;} TOKENELEMENT;

typedef void ( * VFuncSType)(rMoveProc *,char * ) ;
typedef void ( * VFuncIType)(long) ;
typedef void ( * VFuncRType)(MoveTextCursor *) ;
typedef long ( * IFuncIType)(long) ;
typedef long ( * IFuncIIType)(long,long) ;

//extern TOKENELEMENT tokenlist[];  /* found in movepars.c */

enum CMDNR {  nocmd = -100,timecmd,deltaTimecmd,tempocmd,
              setTimecmd,starttimecmd,midioffsetcmd,

              defProc,Let,unDef,doProc,
              mirrorcmd,tracecmd,
              lockposcmd,unlockposcmd,


	      vertangle=500, horangle, rotangle,
	      dvertangle, dhorangle, drotangle,
	      xpos, ypos, zpos,
	      dxpos, dypos, dzpos,
	      rxpos,rzpos,
	      duration,
	      zoom,dist,                     /* Camera values */
	      speed			     /* relative speed */
 };

void installFun(char * fname,enum TOKENTYPE ftype,long faddr);

#endif


