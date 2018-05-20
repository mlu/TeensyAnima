/* drawbody.h
Geometry structures for the body model,
used by model and draw routines

Magnus Lundin  940325 */

#include "movelib.h"

typedef Point3 BODYSHAPE[LASTPOINT];

enum PATCHTYPE { TRIMESH };

typedef struct { int * indexList; int indexCount; /*RGBCOLOR*/int col;enum PATCHTYPE type;} patch;

extern int   patchcount;
extern patch patchlist[];


