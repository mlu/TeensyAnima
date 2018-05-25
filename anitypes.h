/* anitypes.h  */
#ifndef _ANITYPES_H_
#define _ANITYPES_H_

typedef float p3f[3];

typedef float anifloat;

/* This is the header for the library of transformation routines  */

typedef struct	{anifloat xx, xy, xz, yx, yy, yz, zx, zy, zz ;} Matrix3f;

typedef struct	{anifloat w, x, y, z ;} Quatf;

typedef struct	{anifloat x, y, z ;} Point3f;

typedef struct	{int V, H, R ;} VHR;

typedef Point3f Point3;

/*  For floating point version */

#define Matrix3 Matrix3f
#define Quat    Quatf

typedef	VHR	MOVE_POS;
typedef	union	{Quat q; MOVE_POS p;} POS_DATA;

typedef struct	{Point3 Position,PointOfInterest;float Pan,Tilt, Fov,portzoom ;} CAMERAREC;


#endif

