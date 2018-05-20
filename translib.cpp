/*
	v3   060512 All angles in S32Q16 format 
	
	v2   Uses body angles in .1 degees fixpoint format.
	     changeed  convVHR2Q and convQ2VHR
	     used with movebody3.c
*/
/**
*       @todo  Change into a proper right handed coordinate system
*/
#include "translib.h"
//#include "quicksin.h"
#include <math.h>
#include <stdio.h>

#define RADTODEG(v)  (v*57.2957795131)

int convQ2Euler( Point3 * dest, Quat * src, int sequence)
{
	Quat q = * src;
	switch (sequence)
	{
		case 'XZY':
			q.y = src->z;
			q.z = src->y;
		break;
	}

    const anifloat x2 = q.x*q.x;
    const anifloat y2 = q.y*q.y;
    const anifloat z2 = q.z*q.z;
    const anifloat acbd = q.w*q.y - q.x*q.z;
    const anifloat eps = 1e-7;	// TODO: pick from your math lib instead of hardcoding.
    const anifloat pi = 3.14159265358979323846;	// TODO: pick from your math lib instead of hardcoding.
    if (acbd > (0.5-eps))
    {
    	dest->x = 2 * atan2(q.x, q.w);
    	dest->y = pi;
    	dest->z = 0;
    }
    else if (acbd < (-0.5+eps))
    {
    	dest->x = -2 * atan2(q.x, q.w);
    	dest->y = -pi;
    	dest->z = 0;
    }
    else
    {
    	const anifloat adbc = q.w*q.z + q.x*q.y;
    	const anifloat abcd = q.w*q.x + q.y*q.z;
    	dest->x = atan2f(2*abcd, 1 - 2*(y2+z2));
    	dest->y = asinf(2*acbd);
    	dest->z = atan2f(2*adbc, 1 - 2*(x2+y2));
    }

	return 0;
}

int  convQ2VHR( VHR * dest, Quat * src ){
anifloat cv,r2,rh2,v;
int R,V;
cv=2*(src->w*src->w+src->y*src->y)-1.0; /* cosine V */
// Roundoff errors makes arguments slightly out of range
if ( cv > 1.0) cv= 1.0;
// { Message("cv > 1.0 in convQ2VHR");DebugBreak();}
v=acos(cv);
dest->V=V=10*RADTODEG(v);
if (V < 1800 ) {
       r2=atan2( - src->y, src->w );
       R=20*RADTODEG(r2);
       }
else R=0;

if (V > 0) {
      rh2=atan2( src->z, src->x );
      dest->H=R/2 - 10*RADTODEG(rh2);
//      if (dest->H < 1800) dest->H+=3600;
    }
else dest->H=0;

dest->R=(R > 1800)? R-3600:R;
return -1;
}

/* Note Anima uses a left-handed coordinate system */
int convVHR2Q( Quat * dest, VHR * src ){
anifloat sv,cv,sr,cr,srh,crh, rh,v,r;
/* Angles are in 1/10 of degree. Compute integer and fractinal part of angles */
rh=3.1416*(src->R-2*src->H)/3600.0;
v= 3.1416*(src->V)/3600.0;
r= 3.1416*(src->R)/3600.0;

/* Linear interpolation of trigonometric functions */
crh=cosf(rh);srh=sinf(rh);
cv= cosf(v); sv=sinf(v);
cr= cosf(r); sr=sinf(r);

dest->w=  cv*cr;
dest->x=  sv*crh;
dest->y= -cv*sr;
dest->z=  sv*srh;
return 0;
}

int	convVH2Point( Point3f * dest, VHR * src ) {
anifloat sv,cv,sh,ch;
float v,h;
/* Angles are in 1/10 of degree. Compute integer and fractinal part of angles */
v=3.1416*(src->V)/1800.0;
h=3.1416*(src->H)/1800.0;

/* Linear interpolation of trigonometric functions */
ch= cosf(h); sh=sinf(h);
cv= cosf(v); sv=sinf(v);

dest->x=  sv*sh;
dest->y= -cv;
dest->z=  sv*ch;

return 0;
}

int	convPoint2VH( VHR * dest, Point3f * src ){
anifloat h,v,cv;
int H,V;
cv=-src->y;
// Roundoff errors makes arguments slightly out of range
if ( cv >  1.0 ) cv= 1.0 ; //  1.0
if ( cv < -1.0 ) cv=-1.0 ; // -1.0
//{ Message(" src->y out of range in convPoint2VHR");DebugBreak();}
v=acos(cv);
V=10*RADTODEG(v);
if ( (V < 1800 )&& (V>0)) {
       h=atan2( src->x, src->z );
       H=10*RADTODEG(h);
       }
else H=0;

dest->V=V;
dest->H=H;

return 0;
}

int convQ2Mat( Matrix3 * dest , Quat * src ) {
anifloat xx,xy,xz;
anifloat yy,yz,zz;
anifloat xw,yw,zw;
   xx=src->x * src->x;   yy=src->y * src->y;   zz=src->z * src->z;
   xy=src->x * src->y;   xz=src->x * src->z;   yz=src->y * src->z;
   xw=src->x * src->w;   yw=src->y * src->w;   zw=src->z * src->w;
   dest->xx=1-2*(yy+zz);   dest->xy=2*(xy+zw);    dest->xz=2*(xz-yw);
   dest->yx=2*(xy-zw);     dest->yy=1-2*(xx+zz);  dest->yz=2*(yz+xw);
   dest->zx=2*(xz+yw);     dest->zy=2*(yz-xw);    dest->zz=1-2*(xx+yy);
return 0;
}


/********************  ARITHMETIC ROUTINES *******************************/

int mulQuatQuat( Quat * dest, Quat * src1, Quat * src2 ) {
Quat res;
res.w=src1->w*src2->w-src1->x*src2->x-src1->y*src2->y-src1->z*src2->z;
res.x=src1->w*src2->x+src1->x*src2->w+src1->y*src2->z-src1->z*src2->y;
res.y=src1->w*src2->y-src1->x*src2->z+src1->y*src2->w+src1->z*src2->x;
res.z=src1->w*src2->z+src1->x*src2->y-src1->y*src2->x+src1->z*src2->w;
*dest=res;
return 0;
}

int mulQuatFloat( Quat * dest, Quat * src1, anifloat src2) {
dest->w=src1->w*src2;
dest->x=src1->x*src2;
dest->y=src1->y*src2;
dest->z=src1->z*src2;
return 0;
}

int divQuatQuat( Quat * dest, Quat * src1, Quat * src2 ) {
Quat res;
anifloat norm;
norm=src2->w*src2->w+src2->x*src2->x+src2->y*src2->y+src2->z*src2->z;
norm=1;
res.w=(src1->w*src2->w+src1->x*src2->x+src1->y*src2->y+src1->z*src2->z)/norm;
res.x=(-src1->w*src2->x+src1->x*src2->w-src1->y*src2->z+src1->z*src2->y)/norm;
res.y=(-src1->w*src2->y+src1->x*src2->z+src1->y*src2->w-src1->z*src2->x)/norm;
res.z=(-src1->w*src2->z-src1->x*src2->y+src1->y*src2->x+src1->z*src2->w)/norm;
dest->w=res.w;
dest->x=res.x;
dest->y=res.y;
dest->z=res.z;
return 0;
}


/*********************    Transform a whole vector with the same transformation **********/


int trfMat( Matrix3 * angles, Point3 * offset, Point3 ** vector ) {
/* Transforms several Points , using quicksin, and matrix */
register Matrix3 * m=angles;
register Point3 * pp;

int  n;
anifloat dx,dy,dz,tx,ty,tz;


/*//#define NORML / 65536 */

dx=offset->x;dy=offset->y;dz=offset->z;
n=0;
for (pp=vector[n++];(pp != 0L) && (n<60) ;pp=vector[n++]) {

    tx=(m->xx*pp->x+m->xy*pp->y+m->xz*pp->z) ;
    ty=(m->yx*pp->x+m->yy*pp->y+m->yz*pp->z) ;
    tz=(m->zx*pp->x+m->zy*pp->y+m->zz*pp->z) ;

    pp->y=ty + dy;
    pp->x=tx + dx;
    pp->z=tz + dz;
    }
return 0;
}

int trfQuat( Quat * angles, Point3 * offset, Point3 ** vector ){
Matrix3 m;
register Point3 * pp;
int n;
anifloat dx,dy,dz,tx,ty,tz;

convQ2Mat(&m,angles);
dx=offset->x;dy=offset->y;dz=offset->z;
n=0;
for (pp=vector[n++];(pp != 0L) && (n<60) ;pp=vector[n++]) {

    tx=(m.xx*pp->x+m.xy*pp->y+m.xz*pp->z) ;
    ty=(m.yx*pp->x+m.yy*pp->y+m.yz*pp->z) ;
    tz=(m.zx*pp->x+m.zy*pp->y+m.zz*pp->z) ;

    pp->y=ty + dy;
    pp->x=tx + dx;
    pp->z=tz + dz;
    }
return 0;
}

int trfQuatIx( Quat * angles, Point3 * offset, p3f * pointlist,int * ixlist,int listc ){
Matrix3 m;
float * pp;
float dx,dy,dz;
int n;
float tx,ty,tz;
   convQ2Mat(&m,angles);
   dx=((float)offset->x)/1000.0;dy=((float)offset->y)/1000.0;dz=((float)offset->z)/1000.0;
   for (n=0;n<listc;n++) {
      pp=pointlist[ixlist[n]];
      tx=(m.xx*pp[0]+m.xy*pp[1]+m.xz*pp[2]) ;
      ty=(m.yx*pp[0]+m.yy*pp[1]+m.yz*pp[2]) ;
      tz=(m.zx*pp[0]+m.zy*pp[1]+m.zz*pp[2]) ;

      pp[0]=tx + dx;
      pp[1]=ty + dy;
      pp[2]=tz + dz;
   }
return 0;
}


/**************      QUATERNION Exponentiation  and Logarithmation  *******************/

int logQuat( Quat * dest, Quat * src ) {
anifloat theta,sintheta;
anifloat lambda;
/* Correct if *src is a unit quaternion */

  sintheta=sqrtf(1-(src->w)*(src->w));
  if (sintheta >1 ) {
     sintheta=1;//printf("Error in asin \n");
  }
  theta=asinf(sintheta);


  dest->w=0.0;
  if (sintheta>0.0001)
      lambda=theta / sintheta;
  else lambda=1.0;


  dest->x=src->x*lambda;
  dest->y=src->y*lambda;
  dest->z=src->z*lambda;
/*check( dest );*/

return 0;
}



int expQuat( Quat * dest, Quat * src )
{
	Quat res;
	anifloat theta,theta2;
	anifloat lambda;
	
	theta2=(src->x)*(src->x)+(src->y)*(src->y)+(src->z)*(src->z);
	theta =sqrt(theta2);

	if (theta>0.0001)
		lambda= sinf(theta)/theta;
	else
		lambda=1;

	res.w= sin(1.57079632-theta); /*cos(theta)*/;
	res.x=src->x*lambda;
	res.y=src->y*lambda;
	res.z=src->z*lambda;
	*dest=res;
	return 0;
}

