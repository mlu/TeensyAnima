#ifndef _TRANSLIB_H
#define _TRANSLIB_H
#include "anitypes.h"

int	convQ2Mat( Matrix3 * dest , Quat * src );

int	convQ2VHR( VHR * dest, Quat * src );

int	convVHR2Q( Quat * dest, VHR * src );

int	convVH2Point( Point3f * dest, VHR * src );

int	convPoint2VH( VHR * dest, Point3f * src );

int	mulMatMat( Matrix3 * dest, Matrix3 * src1, Matrix3 * src2);

int	mulQuatQuat( Quat * dest, Quat * src1, Quat * src2 );

int	mulQuatFloat( Quat * dest, Quat * src1, anifloat src2);

int	divQuatQuat( Quat * dest, Quat * dividend, Quat * divisor );

int	trfQuat( Quat * angles, Point3 * offset, Point3 ** vector );

int	trfQuatIx( Quat * angles, Point3 * offset, p3f * pointlist,int * ixlist,int n );

int	trfMat( Matrix3 * angles, Point3 * offset, Point3 ** vector );

int	logQuat( Quat * dest, Quat * src );

int	expQuat( Quat * dest, Quat * src );


#endif

