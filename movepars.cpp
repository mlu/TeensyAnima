/* C - library to handle reading of MOVE files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "movelib.h"
#include "movecmd.h"
#include "movetext.h"
//#include "tokenlist.h"
#include "globalsymbols.h"

int ParseParameter(MoveTextCursor * aText,MOVEREC * move,int lvl);
//int ReadToNext(MoveTextCursor * txt );
//TOKENELEMENT * nextToken(MoveTextCursor * aText);
int ParseCommand(MoveTextCursor * aText,MOVEREC * move,int lvl);

extern   long 	thisTime;
static char outs[120];

long TestFlag=0;  /* Special varable to communicate test settings from
		set with command (TESTFLAG nnnn)
		MOVE code to animation system , its meaning changes */
/*************************************************************************/

int  GetMoveInteger(MoveTextCursor * aText,MOVEREC * move );
int  GetMoveText(MoveTextCursor  * aText,MOVEREC * aMoveRec);
int  GetMoveString(MoveTextCursor  * aText,MOVEREC * aMoveRec);

/*************************************************************************/

void installFun(char * fname,enum TOKENTYPE ftype,long faddr){
TOKENELEMENT t;
   t.type=ftype;
   strncpy(t.name,fname,TOKENNAMESIZE);
   t.value=(long)faddr;
   gSymboltable.addToken(&t);
}

/****************************************************************************/


int  ParseCommand(MoveTextCursor * aText,MOVEREC * move,int pnum) {
/* Movement command parser */
/* If pnum == -1  then only a single MOVEREC is returned,
   this is used by eg. getMoveInteger                   */
int np, startLevel;
MOVEREC aMoveRec;
TOKENELEMENT * command, command_values;
const char * cmdStart;

np=pnum+1;
if (pnum<0) pnum=0;
startLevel=aText->getLevel();

if (aText->ReadToNext()) {

cmdStart=aText->getPosition();
command=aText->nextToken();      			/* get command */
command_values=*command;
move[pnum].cmdType=command->type;        /* command type */
move[pnum].value=command->value;         /* command number,bodypart number */
move[pnum].cmdNr=(short)(command->value);

switch (command->type) {
   case	StrCmd:
   case VHRBodyCmd:
   case	XYZBodyCmd:
		if (np <= 0) break;
		{
		TOKENELEMENT * param;
                /*  build pointers to parameters */
                /*  until end of buffer or end of list,unmatched ')' */
		for (param=aText->nextToken(),np=pnum+1;
                   aText->getLevel() > startLevel ;param=aText->nextToken() )
                {
                   if ( param->type == leftP ) { /* recursive subcommand */
                      if (ParseParameter(aText ,move, np)) np++;
                   }  /* recursive evaluation of subcommands */
                   else {
                      if (param->type == ProcFlag ) {
                         move[np].cmdType=(short)(param->type);        /* command type */
                         move[np].cmdNr=(short)(param->value);         /* command number,parameter number */
                         np++;
                      }
	              else {
                         move[np].cmdType=NoToken;move[np].cmdNr=0;   /*  unknown */
                         sprintf(outs,"? in ParseMOVE int cmd expected, time %li ",thisTime); /* potential problem, unparsed data */
			             Message(outs);
                         strncpy(outs,aText->getPosition()-30,35);outs[35]=0;Message(outs);
		      }
                   }
                }   /*  for ( ...  */
		}
                break;
   case  IntCmd:
                GetMoveInteger(aText,move+pnum);
                break;
   case  VoidCmd:
                break;
   case  SysCmd:
     //               np--;
               if (command->value == Let ) {
                  /* WARNING  Maybe it is possible to redefine SYSTEM token values !!!! */
                  TOKENELEMENT te;
                  TOKENELEMENT * param=aText->nextToken();
                  if (param->type == NewSymbol ) {
                     te=*param;
                     param=&te;
                  }
                  if (GetMoveInteger(aText,move+pnum)) {
                     if (param->type == NewSymbol ) param=gSymboltable.addToken(param);
                     param->type=IntToken;
                     param->value=move[pnum].value;
                  }
                  break;
               }
               if (command->value == defProc) {
                  TOKENELEMENT * param=aText->nextToken();
                  if (param->type == NewSymbol ) {
                     param=gSymboltable.addToken(param);
                     param->type=StrCmd;
                     }
                  else if (param->type == StrCmd ) {
		     /* Clean out before redefinition */
                     MoveText * mt;
                     mt=(MoveText *)(param->value);
                     mt->clean();
		  }
                  if (param->type == StrCmd ) {
                     GetMoveText(aText,&aMoveRec);
                     param->value=aMoveRec.value;
                  }
                  break;
               }
               if (command->value == doProc) {
		  GetMoveText(aText,&aMoveRec);
                  ((MoveText *)aMoveRec.value)->autoclean=true;
                  command_values.type=StrCmd;        /* return command type */
                  move[pnum].value=aMoveRec.value;
		  break;
               }
               if (command->value == unDef) {
                  TOKENELEMENT * param=aText->nextToken();
                  if (param->type == StrCmd ) {
                     MoveText * mt;
                     mt=(MoveText *)(param->value);
                     mt->clean();
                  }
                  gSymboltable.removeToken(param);
                  break;
               }
   case VFuncS:{
                  VFuncSType fp;
                  fp=(VFuncSType)(command->value);
                  GetMoveString(aText,&aMoveRec);
                  fp((rMoveProc *)(aText->getOwner()),(char *)(aMoveRec.value));
                  free((char *)(aMoveRec.value));
               }
               break;
   case VFuncI:{
                  VFuncIType fp;
                  fp=(VFuncIType)(command->value);
                  GetMoveInteger(aText,&aMoveRec);
                  fp(aMoveRec.value);
               }
               break;
   case VFuncR:{
  		  VFuncRType fp;
                  fp=(VFuncRType)(command->value);
                  fp(aText);
               }
               break;
   case IFuncI:{
                  IFuncIType fp;
                  long a=0;
                  fp=(IFuncIType)(command->value);
                  if (GetMoveInteger(aText,&aMoveRec)) a=aMoveRec.value;
                  move[pnum].value=fp(a);
                  command_values.type=IntToken;
               }
               break;
   case IFuncII:{
                  IFuncIIType fp;
                  long a=0,b=0;
                  fp=(IFuncIIType)(command->value);
                  if (GetMoveInteger(aText,&aMoveRec)) a=aMoveRec.value;
                  if (GetMoveInteger(aText,&aMoveRec)) b=aMoveRec.value;
                  move[pnum].value=fp(a,b);
                  command_values.type=IntToken;
                }
                break;
   case IntToken: break;
   default:
            command_values.type=NoToken;move[np].cmdNr=0;   /*  unknown */
            sprintf(outs,"WARNING: Unrecognized command\n"); /* potential problem, unparsed data */
            Message(outs);
            strncpy(outs,cmdStart,25);outs[35]=0;Message(outs);
	}
   move[pnum].cmdType=command_values.type;        /* return command type */
   move[pnum].cmdNr=(short)(command_values.value);
   /*  Place a blank row at end of MOVEBLOCK */
   if (np > 0) 	move[np].cmdNr=(move[np].cmdType=EndofBlock);
   /*  Find matching end paranthesis */
   while (aText->getLevel()>startLevel) aText->nextToken();
   return 0;
}
else return 1;
}


int ParseParameter(MoveTextCursor * aText,MOVEREC * move,int pnum) {
/* Movement command parameter parser */
int np,startLevel,res;
TOKENELEMENT * command,command_values;

np=pnum+1;
startLevel=aText->getLevel();
command=aText->nextToken();      			/* get command */
command_values=*command;

switch (command->type) {
/*  PRESENTLY ONLY INTCMD ARE ALLOWED AS PARAMETERS */
  	case  IntCmd:
	    res=GetMoveInteger(aText,move+pnum);
            break;
   	default:
            move[np].cmdType=NoToken;move[np].cmdNr=0;   /*  unknown */
            sprintf(outs,"Unsupported parameter type %i , time=%li\n",command->type, thisTime); /* potential problem, unparsed data */
            Message(outs);
            res=0;
	}
   move[pnum].cmdType=command_values.type;        /* command type */
   move[pnum].cmdNr=(short)(command_values.value);         /* command number,parameter number */

   /*  Find matching end paranthesis */
   while (aText->getLevel()>=startLevel) command=aText->nextToken();

return res;
}


int GetMoveText(MoveTextCursor * aText,MOVEREC * aMoveRec){         // Create a new MoveText - Get text block
	const char *  st, * sl;
	//MoveText * mt;
	int l,startLevel;
    startLevel=aText->getLevel();
    st=aText->getPosition();sl=st;
    while (aText->getLevel()>=startLevel) {
		sl=aText->getPosition() ;
		aText->nextToken();    
	}
    l=sl-st;
    aMoveRec->value=(long)(new MoveText(st,l));
return 0;
}


int GetMoveString(MoveTextCursor * aText,MOVEREC * aMoveRec){         /*  Get text block */
      const char *  st,* sl;
      char * buf;
      int l,startLevel;
      startLevel=aText->getLevel();
      st=aText->getPosition();sl=st;
      while (aText->getLevel()>=startLevel) {
		  sl=aText->getPosition();
		  aText->nextToken();
      }
      l=sl-st;
      buf=(char *)malloc(l+2);
      strncpy(buf,st,l);
      buf[l]=0;
      aMoveRec->value=(long)buf;
      return 0;
}

int  GetMoveInteger(MoveTextCursor * aText,MOVEREC * move) {
/* Integer value parser */
/* int token; */
/* TOKENELEMENT * param; */

/*token=*/ ParseCommand(aText,move,-1);      			/* get value */
   if ( move->cmdType==IntToken ) {
      return 1;
   }
   else  {
      Message("Missing integer argument");strncpy(outs,aText->getPosition()-30,35);outs[35]=0;Message(outs);
      move->cmdType=NoToken;  /* error condition, should be reported */
      return 0;
   }
}

/**
*   Parse the next move command into a MOVEBLOCK structure
*   Called from rmoveproc
*   Recursive parsing is handled by ParseCommand 
*/
int  GetNextMove(MoveTextCursor * aText,MOVEREC * move)
{
    if ((aText->getPosition() == NULL)||(*(aText->getPosition())==0))    /* buffer empty, return */
	    return endBuffer;     /* denna test flyttas till ReadToNext!!! */
           /* Here we put data into *move, or print it for debug */
    return ParseCommand(aText,move,0);
}

