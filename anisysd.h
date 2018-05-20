#ifndef __ANISYSDEF_H
#define __ANISYSDEF_H
/* SYSTEM DEPENDENT DEFS */
/* gcc/g++  */

#ifdef USE_XDISPLAY
#include "sysdisplaytype.h"
#endif

#define VOLID_TYPE    char
typedef struct {int x, y, w, h;} aRect;

/* On UNIX and DOS this can be empty */
#define setVolume(fs)
#define getVolume(fsp)


// Must match size of XPoint
typedef struct {short h, v;} Point;

/* System dependent default color spec's */
#define Colorindex long

/*long TickCount();*/

#define COLORREF unsigned long
#define RGBColor COLORREF
#define Pattern  RGBColor *

#ifdef EJMAC
//Pattern  white  =   0x00FFFFFF;
//Pattern  dkGray =   0x00404040;
#define  gray       0x00808080
//Pattern  ltGray =   0x00C0C0C0;
//Pattern  black  =   0x00000000;
#define  SKIN      0x004080FF
//Pattern  pink   =   0x008080FF;
//Pattern  wine   =   0x00000080;
#define  dkWine     0x00000040
#endif



extern void playMetronome(void);
extern void stopMetronome(void);

extern int gMetronome;
extern int gMetronomeVol;

extern int UnionRect(aRect * dest,aRect * src1, aRect * src2);


#endif

