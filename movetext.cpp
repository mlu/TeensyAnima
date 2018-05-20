#include "movetext.h"
#include "globalsymbols.h"
//#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
 
MoveText::MoveText(const char * buf,int len) {
      active_count = 0;
      autoclean=false;
      //inactive=0;
      text=(char *)malloc(len+2);
      strncpy(text,buf,len);
      text[len]=0;
      length=len;
}
   
MoveText::~MoveText() {
     if (text) {
       free(text);
     }
}
   
MoveText * MoveText::clean() {
	if (active_count<=0) {
		delete this;
		return NULL;
	}
	else {
		autoclean=true;
		//inactive=1;
		return this;
	}
}

void  MoveText::incRef() {
	active_count++;
}

void  MoveText::decRef() {
	active_count--;
	if ((active_count <= 0) && autoclean) {
		delete this;
	}
}


void MoveText::copyAsBuffer(char * buf) {
	int len=strlen(buf);
	text = (char *)malloc(len+2);
	length=len;
	//free_buffer=1;
	//inactive=0;
	strncpy(text,buf,len);
	text[len]=0;
}


#define READTOEOL(TextBufPtr)  while ( (*(TextBufPtr)>0)&&(*(TextBufPtr) !='\r')&&(*(TextBufPtr) !='\n') ) (TextBufPtr)++;

int MoveTextCursor::ReadToNext() {
/* Reads in  txt buffer, bpos until next kommando, justerar level,
   ger length av kommandot, exkl. parametrar tillbaka. */
unsigned char ch;
int l;
    const char * bpos=getPosition();
    while ( (ch=*(bpos) )<=32 || (ch == ';') || (ch == '(') || (ch == ')') )
    {
        switch (ch) {
	       case '(': incLevel(); break;
	       case ')': decLevel(); break;
	       case 0  : {
	                   setPosition(bpos);
                           return 0;                /* End of buffer */
                         }
	       case ';': READTOEOL(bpos)  ;  /* Skip comments */
           default : break;
        }
        (bpos)++;
    }
    setPosition(bpos);
    l=1;
    while (*(bpos+l)>32) l++;
    return l;
}

#define ToUpper(ch) ((islower(ch))?(char)(toupper(ch)):ch)

TOKENELEMENT * MoveTextCursor::nextToken() {
	static TOKENELEMENT  thisToken;
	//static int           thisTokenN;

	TOKENELEMENT  * pToken;
	char c,*cp, ord[45];
	const char * buf;
	long val;
	short n;

	buf=getPosition();
	pToken=&thisToken;

	while (((c=*buf) <=' ') && (c !=0) ) (buf)++;  /* Skip white space */

	switch (c) {
		case '(': 
			(buf)++;/* strcpy(thisToken.name,"LEFTP"); */
			thisToken.type=leftP;
			incLevel();
			break;
		case ')': 
			(buf)++; /* strcpy(thisToken.name,"RIGHTP"); */
			thisToken.type=rightP;
			decLevel();
			break;
		case 0  : 
			thisToken.type=endBuffer;
			setLevel(-1);
			break;
		case ';':
			READTOEOL(buf);     /* Read to end of line */
			/* strcpy(thisToken.name,"COMMENT"); */
			thisToken.type=Comment;
			break;
		default:
			cp=ord;                              /* Read symbol text string */
			*(cp++)=ToUpper(c); c=*(++(buf));    /* at least on char should be read */
			/* but not more then size(ord) = 40  */
			for (*cp=ToUpper(c),n=0 ; (c>')') && (n<40); c=*(++(buf)),n++) *(cp++)=ToUpper(c);
			*cp=0;
			/* This is slow, its faster to check for a leading +-09 */
			if (sscanf((char *)ord,"%ld",&val)==1) {  /* Found a number, decimal */
				thisToken.value=val;
				/*		 strcpy(thisToken.name,"NUMBER");  */
				thisToken.type=IntToken;
				break;
			}
			if ( (pToken=gSymboltable.FindToken(ord))==NULL ) {
				thisToken.value=0;
				strncpy(thisToken.name,(char *) ord,TOKENNAMESIZE);
				thisToken.type=NewSymbol;
				pToken=&thisToken;
			}
	}

	while ( (*buf <= ' ') && (*buf !=0) ) buf++; /* Then skip blanks etc */
	setPosition(buf);                     /* new buffer pointer */
	return pToken;                           /* return token type nr */
}

