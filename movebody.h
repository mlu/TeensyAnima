#ifndef MOVEBODY_H
#define MOVEBODY_H

#include "movelib.h"
#include "cinterpo.h"

typedef struct {
    long	thisTime;
    int		size;
    CInterpolationObject	jointData[LASTPART];
    POS_DATA				jointPosQ[LASTPART];
} MOVEBODY;


int dist360(int a,int b) ;
int mod360(int a);
void normalizeVHR(VHR * angles);
void SetBodyMove(MOVEBLOCK  theMove,MOVEBODY * aBody,long atTime,long localSpeed) ;

#endif





