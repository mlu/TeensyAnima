#ifndef _MOVETEXT_
#define _MOVETEXT_
#include <unistd.h>
#include "movelib.h"
#include "anisysd.h"
#include "rtproc.h"
//#include "movecmd.h"
#include "globalsymbols.h"
//#include <stdlib.h>

/* Convert this to a proper class */

class MoveText  {
friend class MoveTextCursor;
   long      active_count;
   //int       inactive;
   long      length;
   char      * text;
   virtual ~MoveText();

public:
   bool      autoclean;

   MoveText() {
      active_count = 0;
      autoclean=false;
      //inactive=0;
      length=0;
      text=NULL;
   }

   /* Create new MoveText and duplicate buffer */
   MoveText(const char * buf,int len);

   void  incRef();
   void  decRef();
   //int getActiveCount(){return active_count;};
   //bool isActive(){return (!inactive);};
   MoveText * clean(); 
   void copyAsBuffer(char * buf);
   void setBuffer(char * buf,int len) { text=buf; length=len; };

};


class MoveTextCursor {
  /* Handle cursor position and command level within a MoveText buffer */ 
  MoveText * movetext;
  char * buffer;
  char * bpos ;
  short level;
  RTProcess * owner /* Usually a (rMoveProc *) */;

 public:

   MoveTextCursor(MoveText * text,RTProcess * _owner) {
      movetext = NULL;
      setText(text);
      owner=_owner;
   }

   bool isLoaded() {
      return (movetext != NULL) ; 
   }
   void setText(MoveText * text) {
      if (movetext) {
         movetext->decRef();      }
      movetext = text;
      if (movetext) {
         buffer = text->text;
         text->incRef();
      }
      else {
         buffer = NULL;      }
      reset();
   }
   
   void reset() {
      bpos = buffer;
      level=0;
   }   
   ~MoveTextCursor() {
     if (movetext) 
       movetext->decRef();
   }
   
   RTProcess * getOwner(){return owner;};
   int    getLevel(){return level;};
   void   setLevel(int lvl){level=lvl;};
   void   incLevel(){level++;};
   void   decLevel(){level--;};
   
   const char * getPosition(){return bpos;};
   void setPosition(const char * pos){bpos=(char *)pos;};       /* CHECK THIS */
      void setBufferStart(const char * pos) {buffer=bpos;};  /* CHECK THIS */
   
   int ReadToNext();   TOKENELEMENT * nextToken();
};

/*
typedef struct  {
   long      active_count;
   int       free_on_exit;
   int       inactive;
   long      textlength;
   char      text[1];
}  MOVETEXT;

typedef struct {
   char * buffer;
   char * bpos ;
   short level;
   char * owner; // Usually a (rMoveProc *) 
}  MOVEREADPOS;
*/

// initfile.cpp
//extern MOVETEXT *  InitFile(char * fname,VOLID_TYPE * fileSpec);
extern MoveText *  InitFile(char * fname,VOLID_TYPE * fileSpec);


// moveparse.c
//int GetNextMove(MOVEREADPOS * aText,MOVEBLOCK aMove);

#endif

















