#if !defined	(TRANSFORM_H)
#define			 TRANSFORM_H

struct VECTOR
{
  float x, y, z;
};

struct TrackballStruct
{
	float  centerX, centerY;   // sphere center
	float  radius ;            // sphere radius   
	float  pointX1, pointY1;   // the first  mouse  position
	float  pointX2, pointY2;   // the second mouse  position
	float  pointX3, pointY3;   // release mouse     position
};

struct EYE
{
  VECTOR origin;  // eye point
  VECTOR center;  // refrence point
  VECTOR up;      // up vector
  VECTOR axisX;   // X-Axis vector of the eye coordinate
  VECTOR axisY;   // Y-Axis vector of the eye coordinate
  VECTOR axisZ;   // Z-Axis vector of the eye coordinate
};

//---------------- vector & matrix operation -----------------------

// V = ( x, y, z )  or  V = A 
void VectorCopy(VECTOR & V, float x, float y, float z);
void VectorCopy(VECTOR & V, VECTOR A);

// V = A + B
void VectorAdd(VECTOR & V, VECTOR A, VECTOR B);

// V = A - B
void VectorSub(VECTOR & V, VECTOR A, VECTOR B);

// V = s * V
void VectorScale(VECTOR & V, float s);

// V = A x B
void VectorCross(VECTOR & V, VECTOR A, VECTOR B);

// dot = A . B
void VectorDot(float & dot, VECTOR A, VECTOR B);

// len = |V|
void VectorLength(float & len, VECTOR V);

// V = V/(|V|)
void VectorNormalize(VECTOR & V);

// M = M1*M2
void MultMatrixf( float *M,  float *M1, float *M2,
					 int m1Row, int m1Col, int m2Row, int m2Col);

// transpose matrix
void TransMatrix( float *M, int row, int col);

// "inverse" of matrix M
int  InvMatrixf(float *M, int n);

//---------------- world coordinate -------------------------------------

// transform point P(x,y,z) from object coordinate to world coordinate
void ObjectToWorld(float point[3], float result[3]);

// transform point P(x,y,z) from world coordinate to object coordinate
void WorldToObject(float point[3], float result[3]);

// translate in world coordinate, d[3]=(dx, dy, dz)
void WorldTranslate(float d[3]); 

// rotate in world coordinate
void WorldRotate(float point[3], float normal[3], float angle);

//----------------- eye coordinate -------------------------------------- 
// eye is in the world coordinate

// calculate eye axes vectors
void SetEyeStruct(float eyeX,    float eyeY,    float eyeZ, 
					 float centerX, float centerY, float centerZ, 
					 float upX,     float upY,     float upZ,
					 EYE  & eye);

// transform point P(x,y,z) from world coordinate to eye coordinate
void WorldToEye(float point[3], float result[3], EYE eye);

// transform point P(x,y,z) from eye coordinate to world coordinate
void EyeToWorld(float point[3], float result[3], EYE eye);

// translate in eye coordinate, d[3]=(dx, dy, dz)
void EyeTranslate(float d[3], EYE eye);

// rotate in eye coordinate
void EyeRotate(float point[3], float normal[3], float angle, 
				  EYE eye);

//TrackBall Algorithm
void Trackball(int   event,   float radius ,
				  float centerX, float centerY, float mouseX, float mouseY,
				  float &axisX , float &axisY , float &axisZ, float &angle );

#endif