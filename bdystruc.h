/* Structures and data for joint angle restrictions, mirroring
   and linking coordinate frames to different bodyparts       */
/* 94 03 25  Magnus Lundin  */

typedef struct {
	int parent;        /* Normal, physical, parent bodypart */
	int child;			/* Child part */
	int next;			/* Next part on same level */
	int mirrorpart;    /* Mirror bodypart  */
	int minV, maxV;
	int minH, maxH;
	int minR, maxR;
	}
JOINTSTRUCTREC;

/* The parentLink[] array goes into the mBodyProcess object  */
/* When initializing we always set  parentLink[]=BodyStuct[].parentPart  */
/* Here we store current part to geometrically relate to */

/*extern int[LASTPART] parentLink;*/

/* Mirroring is done by mapping
   part --->  BodyStruct[part].mirrorpart
and changing angles to
      H ---> -H
      R ---> -R
or for quaternions
      y ---> -y
      z ---> -z
or in XYZ euclidean coordinates
      X ---> -X
*/

#define MAX 32000

/* The BodyStruct is static and internal to the interpolation/modelling code */
JOINTSTRUCTREC BodyStruct[LASTPART]={
{-1,		BODYTURN, 	-1, -1,   0,   0,   0,   0,   0 },/*BODYLOCATION = FIRSTPART = 0*/
{BODYTURN,	PELVIS,		-1, -1,   0,   0,   0,   0,   0 },	/*PELVISLOC*/
{BODYLOCATION,	PELVISLOC,	-1, -1,   0,   0,   0,   0,   0 },   /*BODYTURN*/
{PELVISLOC,	RHIP,		-1, -1  -MAX, MAX,-MAX, MAX,-MAX, MAX},  /* PELVIS */
{PELVIS,	UPPERBACK,	-1, -1, -90,  90,-MAX, MAX, -40,  40 },      /* WAIST  */
{WAIST,		NECK,		RSHOULDER, -1, -90,  90,-MAX, MAX, -40,  40},     /* UPPERBACK */
{UPPERBACK,	HEAD,		-1, -1,  -90,  90,-MAX, MAX, -40,  40},          /* NECK */
{NECK,		-1,			-1, -1,  -90,  90,-MAX, MAX, -40,  40},          /*HEAD*/
{PELVIS, 	RKNEE,		LHIP, LHIP, -MAX, MAX,-MAX, MAX, -90,  90},             /* RHIP */
{RHIP, 		RANKLE,		-1, LKNEE,  -170,   0,   0,   0,   0,   0},            /* RKNEE */
{RKNEE, 	RTOE,		-1, LANKLE, -170,   0,   0,   0,   0,   0},            /* RANKLE */
{RANKLE,	-1,			-1, LTOE,   -170,   0,   0,   0,   0,   0},            /* RTOE */
{PELVIS,	LKNEE,		-1, RHIP, -MAX, MAX,-MAX, MAX, -90,  90},             /* LHIP */
{LHIP,		LANKLE,		-1, RKNEE,  -170,   0,   0,   0,   0,   0},            /* LKNEE */
{LKNEE, 	LTOE,		-1, RANKLE, -170,   0,   0,   0,   0,   0},            /* LANKLE */
{LANKLE,	-1,			-1, RTOE,   -170,   0,   0,   0,   0,   0},            /* LTOE */
{UPPERBACK,	RARM,   LSHOULDER,LSHOULDER,  -60,   60,  90,  90, -60,  60},      /* RSHOULDER */
{RSHOULDER, RELBOW,		-1, LARM,  -MAX, MAX,-MAX, MAX, -90,  90},             /* RARM */
{RARM, 		RWRIST,		-1, LELBOW,  -170,   0,   0,   0,   0,   0},            /* RELBOW */
{RELBOW,	RFINGERS, 	-1, LWRIST, -170,   0,   0,   0,   0,   0},            /* RWRIST */
{RWRIST,	-1,			-1, LTHUMB,  -170,   0,   0,   0,   0,   0},            /* RTHUMB */
{RWRIST,	-1,			RTHUMB, LFINGERS, -170,   0,   0,   0,   0,   0},            /* RFINGERS */
{UPPERBACK, LARM,	-1, RSHOULDER,  -60,   60,  90,  90, -60,  60},  /* LSHOULDER */
{LSHOULDER,	LELBOW,		-1, RARM,  -MAX, MAX,-MAX, MAX, -90,  90},   /* LARM */
{LARM,		LWRIST,		-1, RELBOW,-170,   0,   0,   0,   0,   0},            /* LELBOW */
{LELBOW,	LFINGERS,	-1, RWRIST,-170,   0,   0,   0,   0,   0},            /* LWRIST */
{LWRIST,	-1,			-1, RTHUMB,-170,   0,   0,   0,   0,   0},            /* LTHUMB */
{LWRIST,	-1,			LTHUMB, RFINGERS,-170,   0,   0,   0,   0,   0}            /* LFINGERS */
};





