// matrix.cpp: implementation of the VolumeRenderingSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MATRIX.h"
#include "math.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
static void SetMatric(double pMatric[3][3], double angle, int index)
{
	
}
void AssembleMatric(double dMatric[3][3], double dAngle[3])
{
	double m0[3][3], m1[3][3], m2[3][3], tm[3][3];
	SetMatric(m0, dAngle[0], 0);
	SetMatric(m1, dAngle[1], 1);
	SetMatric(m2, dAngle[2], 2);
	MATRIX<double, double>::Multiple(tm, m0, m1);
	MATRIX<double, double>::Multiple(dMatric, tm, m2);
}

void VectorToDmatrix(double vector[3],double dmatrix[3][3], int changewnd, int prewnd)
{
	
}
