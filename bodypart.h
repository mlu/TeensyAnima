/*Header file with definitions for standard body used with MOVE files */
#ifndef _BODYPART_
#define _BODYPART_

/* #include "quat.h" */

/* Each BODYPART corresponds to a command that moves it */
/* The corresponding shapes are defined elswhere        */
/* THIS STRUCTURE MUST BE SYNCHRONIZED WITH THE SHAPE IN MODEL.C */
enum BODYPART { FIRSTPART=0,
		BODYLOCATION=0,        /* pelvis space coordinates */
		PELVISLOC,
		BODYTURN,
		PELVIS,                /* bodycenter */
		WAIST,
		UPPERBACK,
		NECK,
		HEAD,
		RHIP,
/*		RHIP_ROT,  */     /* Virtual joint for hip rotation */
		RKNEE,
		RANKLE,
		RTOE,
		LHIP,
/*		LHIP_ROT, */       /* Virtual joint for hip rotation */
		LKNEE,
		LANKLE,
		LTOE,
		RSHOULDER,
		RARM,
/*		RARM_ROT,  */     /* Virtual joint for upper arm rotation */
		RELBOW,
/*		RELBOW_ROT, */      /* Virtual joint for lower arm rotation */
		RWRIST,
		RTHUMB,
		RFINGERS,
		LSHOULDER,
		LARM,
/*		LARM_ROT,    */   /* Virtual joint for upper arm rotation */
		LELBOW,
/*		LELBOW_ROT,  */       /* Virtual joint for lower arm rotation */
		LWRIST,
		LTHUMB,
		LFINGERS,
		LASTPART,
		NOSE,
		HEADCENTER,
		CROWN,
		REYE,
		LEYE,
		MSHOULDER,
		RBREAST,
		LBREAST,
		RWAIST,
		LWAIST,
		RBUTTOCK,
		LBUTTOCK,
		RTHIGH,
		LTHIGH,
		RFINGER1,
		RFINGER4,
		RTHUMBNAIL,
		LFINGER1,
		LFINGER4,
		LTHUMBNAIL,
		LASTPOINT
};






#endif

















