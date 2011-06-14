#ifndef DEFS_H

#define DEFS_H
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef float Coord3[3];
typedef float Vector3[3];
typedef float Matrix4x4[4][4];
typedef int	  Triangle[3];
typedef short ShortArray3[3];
typedef	float FRGB[3];
typedef int   int2[2];
typedef int	  *Pint;


#define Size_int2   sizeof(int2)
#define	Size_Coord3	sizeof(Coord3)
#define	Size_FRGB	sizeof(FRGB)
#define	Size_Vector3	sizeof(Vector3)
#define	Size_Matrix4x4	sizeof(Matrix4x4)
#define	Size_Triangle	sizeof(Triangle)
#define Size_short	sizeof(short)
#define Size_int	sizeof(int)
#define	Size_float	sizeof(float)
#define M_PI		3.141592653589793238462643383280
#define MAXFLOAT	0x7fffffff
#endif