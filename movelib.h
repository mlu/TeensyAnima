/*Header file with definitions for standard body used with MOVE files */
/*91 11 20  	New version to give support for quaternion interpolation */

#ifndef _MOVELIB_
#define _MOVELIB_


#include "anisysd.h"

#include "bodypart.h"
#include "anitypes.h"

struct _VHR {short V,H,R;};

typedef  MOVE_POS BODYPOS[LASTPART];

typedef struct { short cmdType; short cmdNr; long value; } MOVEREC;

typedef MOVEREC * MOVEBLOCK;

typedef POS_DATA  BODYANGLES_Q[LASTPART];

typedef struct {
	CAMERAREC lastCamera;
	CAMERAREC nextCamera;
	CAMERAREC thisCamera;
	long	lastTime;
	long	nextTime;
	long	thisTime;
} MOVECAMERA;

typedef struct { short r,g,b ;} RGBCOLOR;

typedef struct {
	RGBCOLOR color;
	Colorindex ci;
	float amdient,diffuse,specular;
	int flag;
} MATERIAL_TYPE;

typedef MATERIAL_TYPE * MATERIALP;

#define SELECTED_FLAG 1
#define MA_NOSHOW     2

typedef struct {
	MATERIAL_TYPE * material;
	int  flag;
} DANCER_ATTRIBS;

//#include "movetext.h"
class MoveTextCursor;

/********************** Function prototypes ******************************/
//#ifdef __cplusplus
//extern "C" {
//#endif
/************ interface routines ************************/

int Message(const char * msg);


/***   movepars.c   ***/
int GetNextMove(MoveTextCursor * aText,MOVEBLOCK aMove);



/***   movebody.c   ***/

void MirrorBodyMove(MOVEBLOCK  theMove) ;
      /* Change a move to its left-right mirror image  */


/***  model.c  ***/

void ModelBody_Q(BODYANGLES_Q aBodyPos, Point3 * * aShape,anifloat * lockxp,anifloat * lockzp,int * locknrp ) ;
/*  Generate geometry data, to be sent to rendering procedure */

void CameraTransform(CAMERAREC * aCamera,Point3 * inP ,int nP,Point3 * outP);
/* Transforms several Points , using quicksin */

void CameraProject(CAMERAREC * aCamera,Point3 * inP ,int nP,Point * outP);
/* Projects several Point3  */

void CameraTransProj(CAMERAREC * aCamera,Point3 * inP ,int nP,Point * outP);
/* Transforms and projects several Points , using quicksin */

#endif


