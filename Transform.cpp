#include "stdafx.h"
#include "Transform.h"
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>


//---------------- vector & matrix operation -----------------------

// V = ( x, y, z )
void VectorCopy(VECTOR & V, float x, float y, float z)
{
   V.x = x;
   V.y = y;
   V.z = z;
}

// V = A 
void VectorCopy(VECTOR & V, VECTOR A)
{
   V.x = A.x;
   V.y = A.y;
   V.z = A.z;
}

// V = A + B
void VectorAdd(VECTOR & V, VECTOR A, VECTOR B)
{
  V.x = A.x + B.x;
  V.y = A.y + B.y;
  V.z = A.z + B.z;
}

// V = A - B
void VectorSub(VECTOR & V, VECTOR A, VECTOR B)
{
  V.x = A.x - B.x;
  V.y = A.y - B.y;
  V.z = A.z - B.z;
}

// V = s * V
void VectorScale(VECTOR & V, float s)
{
   V.x = s * V.x;
   V.y = s * V.y;
   V.z = s * V.z;
}

// V = A x B
void VectorCross(VECTOR & V, VECTOR A, VECTOR B)
{
  V.x = A.y*B.z - A.z*B.y;
  V.y = A.z*B.x - A.x*B.z;
  V.z = A.x*B.y - A.y*B.x;
}

// dot = A . B
void VectorDot(float & dot, VECTOR A, VECTOR B)
{
  dot = A.x*B.x + A.y*B.y + A.z*B.z;
}

// len = |V|
void VectorLength(float & len, VECTOR V)
{
   len = sqrt(V.x*V.x + V.y*V.y + V.z*V.z);
}

// V = V/(|V|)
void VectorNormalize(VECTOR & V)
{
  float len;

  len = sqrt(V.x*V.x + V.y*V.y + V.z*V.z);

  if(len<=0.0000001f)
    {
	  V.x = 0.0f;
	  V.y = 0.0f;
	  V.z = 0.0f;
    }
  else
    {
	  V.x /= len;
	  V.y /= len;
	  V.z /= len;
    }
}

// M = M1 * M2
void MultMatrixf( float *M,  float *M1, float *M2,
					 int m1Row, int m1Col, int m2Row, int m2Col)
{
  if ( m1Col != m2Row )  return ;
  int size = m1Row * m2Col;
  
  for( int i=0, i1=0; i<size; i+= m2Col, i1+= m1Col)
    for( int j = 0; j < m2Col; j++)
    {
      M[i+j] = 0.0f;
      for( int k=0, k1=0; k<m1Col; k++, k1+=m2Col)
	  M[i+j] = M[i+j] + M1[i1+k] * M2[k1+j];
    }                         
}

//------------------  inverse  -------------------------
static int LUDcmp( float *data, int *index, int n)
{
  const float TINY = 1.0e-20f;
  float *vv= new float [n];
  float temp, big;
  int i, ii, j, jj, k, kk, imax = 0;

  for (i = 0, ii= 0; i < n; i++, ii += n)
  {
    big = 0.0f;
    for(j = 0; j < n; j++)
 	   if((temp = fabs( data[ii+j])) > big)  big = temp;
       
	if( big == 0.0f )
    {
      delete vv;
	  return 0;
    }
     
	vv[i] = 1.0f /big;
  }
  for( j = 0, jj = 0; j < n; j++, jj += n)
  {
     for( i = 0, ii = 0; i <= j; i++, ii += n)
     {
       temp = data[ii+j];
       for( int k = 0, kk = 0; k < i; k++, kk += n)
	       temp -= data[ii+k] * data[kk+j];
       data[ii+j] = temp;
     }
     big = 0.0f;
     imax = j;
     for ( i = j+1, ii = i * n; i < n; i++, ii += n)
     {
	   temp = data[ii+j];
	   for( k = 0, kk = 0; k < j; k++, kk += n)
	      temp -= data[ii+k] * data[kk+j];
	   data[ii+j] = temp;
	   if(( temp = vv[i] * fabs(temp)) > big)
	   {
	     big = temp;
	     imax = i;
        }
     }
     if( j != imax)
     {
	   ii = imax * n;
	   for ( k = 0; k < n; k++)
	   {
	   temp = data[ii+k];
	   data[ii+k] = data[jj+k];
	   data[jj+k] = temp;
	   }
	   vv[imax] = vv[j];
     }
     index[j] = imax;
     if( data[jj+j] == 0.0f ) data[jj+j] = TINY;
     if( j < n)
     {
	   temp = 1.0f/data[jj+j];
	   for ( i = j+1, ii = i * n; i < n; i++, ii += n)
	   data[ii+j] *= temp;
     }
   }
   delete vv;
   return 1;
}

void LUBksb( float *data, int *index, float *b, int n)
{
  int ii, i, j, k = -1, ip;
  float sum;
  for ( i = 0, ii = 0; i < n; i++, ii += n)
  {
    ip = index[i];
    sum = b[ip];
    b[ip] = b[i];
    if ( k != -1)
      for ( j = k; j < i; j++)    sum -= data[ii+j]*b[j];
    else if ( sum)
      k = i;
    b[i] = sum;
  }
  for( i = n-1, ii = i * n; i >=0; i--, ii -= n)
  {
    sum = b[i];
    for( j = i + 1; j < n; j++)   sum -= data[ii+j] * b[j];
    b[i] = sum / data[ii+i];
  }
}

int InvMatrixf(float *data, int n)
{
  int *index = new int [n];
  if ( LUDcmp( data, index, n) == 0)
  {
     delete index;
     return 0;
  }
  float *b = new float [n];
  float *inva = new float [n*n];
  for ( int i = 0; i < n; i++)
  {
    for( int j = 0; j < n; j++)   b[j] = 0.0f;

    b[i] = 1.0f;
    LUBksb( data, index, b, n);
    int j,jj;
    for ( j = 0, jj = 0; j < n; j++, jj += n)   inva[jj+i] = b[j];
  }
  for (int i = 0; i < n*n; i++)   data[i] = inva[i];
  delete b;
  delete inva;
  delete index;
  return 1;
}

// transpose matrix
void TransMatrix( float *M, int m, int n)
{
   float *temp = new float [m*n];
   for( int i = 0, ii = 0; i < m; i++, ii += m)
   {
     for( int j = 0, jj = 0; j < n; j++, jj += n)
        temp[ii+j] = M[jj+i];
   }

   for(int i = 0; i < m*n; i++)  M[i] = temp[i];

   delete temp;
   return;
}


//-------------------------------------------------------
// 跟踪球算法实现
//-------------------------------------------------------
// 参考 《科学计算可视化算法与系统》 P232 - P233
//-------------------------------------------------------
// 周昆，1998，3，23

void Trackball(int   event,   float radius ,
				  float centerX, float centerY, float mouseX, float mouseY,
				  float &axisX , float &axisY , float &axisZ, float &angle )
{

	//------ TB trackball ----
	static TrackballStruct TB;
  
	if(event==0)  // TB_START
	{
		TB.centerX = centerX; 
		TB.centerY = centerY; 
		TB.radius  = radius ;
		TB.pointX1 = mouseX ;   // !
		TB.pointY1 = mouseY ;   // !
		return;
	}
	if(event>0)  // TB_MOVE , TB_END
	{
		float sinTao, cosTao, sinOmiga, cosOmiga, sinSita, cosSita;

		TB.centerX = centerX; 
		TB.centerY = centerY; 
		TB.radius  = radius ;
		TB.pointX2 = mouseX ;   // !
		TB.pointY2 = mouseY ;   // !

		VECTOR  V1, V2, V3; 
		float len1, len2, len3;
		float dot;
		float dx,dy;

		// 计算 sinTao, cosTao
		dx = TB.pointX2 - TB.pointX1;
		dy = TB.pointY2 - TB.pointY1;
		VectorCopy(V1, dx, dy, 0.0f);
  
		dx = TB.pointX1 - TB.centerX;
		dy = TB.pointY1 - TB.centerY;
		VectorCopy(V2, dx, dy, 0.0f);

		VectorCross(V3, V2, V1);
		VectorLength(len1, V1);
		VectorLength(len2, V2);
		VectorLength(len3, V3);
  
		if( len1<=0.00001f  || len2<=0.00001f )
		{ 
			sinTao = 0.0f;
			cosTao = 1.0f;
		} 
		else
		{
			if( V3.z>=0.0f ) sinTao =  len3/(len1*len2);
			else             sinTao = -len3/(len1*len2); 
			VectorDot(dot, V1, V2);
			cosTao = dot/(len1*len2);
		}

		// 计算sinOmiga,cosOmiga
		sinOmiga  = sqrt(dx*dx + dy*dy);
		sinOmiga  = sinOmiga/TB.radius;
		if(sinOmiga>1.0f)
			sinOmiga=1.0;
		//99/12/28
		sinOmiga = 0.0f;
		cosOmiga=sqrt(1.0-sinOmiga*sinOmiga);

		//计算sinSita,cosSita
		len1 = sqrt(dx*dx + dy*dy);
		if(len1<=0.00001f)
		{  
			sinSita = 0.0f;
			cosSita = 1.0f;
		}
		else
		{
			sinSita = dy/len1;
			cosSita = dx/len1;
		}

		// 计算旋转角度，用《角度》表示
		dx = TB.pointX2 - TB.pointX1;
		dy = TB.pointY2 - TB.pointY1;
		len1= sqrt(dx*dx + dy*dy);
		angle = (len1/TB.radius)*45.0f;


		TB.pointX1 = TB.pointX2;
		TB.pointY1 = TB.pointY2;

		float M [1][3] = { 0.0f,     0.0f,    0.0f      };
		float M0[1][3] = { 0.0f,     0.0f,    0.0f      };
		float M1[1][3] = {-sinTao,   cosTao,  0.0f      };
		float M2[3][3] = { cosOmiga, 0.0f,   -sinOmiga,
							0.0f,     1.0f,   0.0f,
							sinOmiga, 0.0f,   cosOmiga  };
		float M3[3][3] = { cosSita,  sinSita, 0.0f,
							-sinSita,  cosSita, 0.0f,
							0.0f,     0.0f,    1.0f      };

		MultMatrixf( &M0[0][0], &M1[0][0], &M2[0][0], 1, 3, 3, 3);
		MultMatrixf( &M [0][0], &M0[0][0], &M3[0][0], 1, 3, 3, 3);

		//----- output -----------
		axisX = M[0][0];
		axisY = M[0][1];
		axisZ = M[0][2];
	} // end of TB_MOVE 
}

// transform point P(x,y,z) from object coordinate to world coordinate
void ObjectToWorld(float point[3], float result[3])
{
	// (1) get current modelview matrix
	float temp[4][4];
	glGetFloatv(GL_MODELVIEW_MATRIX, &temp[0][0]);

	// (2) transpose the modelview matrix
	TransMatrix( &temp[0][0], 4, 4);

	float a[4][1];
	float b[4][1];
	a[0][0]=point[0];
	a[1][0]=point[1];
	a[2][0]=point[2];
	a[3][0]=1.0f ;
    
	// (3) transform (x,y,z) from object coordinate to world coordinate    
    MultMatrixf( &b[0][0], &temp[0][0], &a[0][0], 4, 4, 4, 1);
    result[0]=b[0][0];
    result[1]=b[1][0];
    result[2]=b[2][0];
}

// transform point P(x,y,z) from world coordinate to object coordinate
void WorldToObject(float point[3], float result[3])
{
  // (1) get current modelview matrix
  float temp[4][4];
  glGetFloatv(GL_MODELVIEW_MATRIX, &temp[0][0]);

  // (2) transpose the modelview matrix
  TransMatrix( &temp[0][0], 4, 4);

  // (3) inverse the modelview matrix
  if( InvMatrixf(&temp[0][0], 4) )
  {
	float a[4][1];
	float b[4][1];
	a[0][0]=point[0];
	a[1][0]=point[1];
	a[2][0]=point[2];
	a[3][0]=1.0f ;
    
	// (4) transform (x,y,z) from world coordinate to object coordinate    
    MultMatrixf( &b[0][0], &temp[0][0], &a[0][0], 4, 4, 4, 1);
    result[0]=b[0][0];
    result[1]=b[1][0];
    result[2]=b[2][0];
  }
}

// rotate in world coordinate
void WorldRotate(float point[3], float normal[3], float angle)
{
	float pp[3];
	float nn[3];
    float tt[3];

	float t[3]={0.0f,0.0f,0.0f};
	WorldToObject(point, pp);
	WorldToObject(normal, nn);
	WorldToObject(t, tt);
	nn[0]-=tt[0];
	nn[1]-=tt[1];
	nn[2]-=tt[2];
	glTranslatef(pp[0],pp[1],pp[2]);
	glRotatef(angle,nn[0],nn[1],nn[2]);
	glTranslatef(-pp[0],-pp[1],-pp[2]);
}

// translate in world coordinate
void WorldTranslate(float p[3])
{
  float pp[3];
  float tt[3];

  float t[3]={0.0f,0.0f,0.0f};
  WorldToObject(p, pp);
  WorldToObject(t, tt);
  pp[0]-=tt[0];
  pp[1]-=tt[1];
  pp[2]-=tt[2];
  glTranslatef(pp[0],pp[1],pp[2]);
}

// calculate eye axes vectors
void SetEyeStruct(float eyeX,    float eyeY,    float eyeZ, 
					 float centerX, float centerY, float centerZ, 
					 float upX,     float upY,     float upZ,
					 EYE  & eye)
{
  eye.origin.x=eyeX;
  eye.origin.y=eyeY;
  eye.origin.z=eyeZ;
  eye.center.x=centerX;
  eye.center.y=centerY;
  eye.center.z=centerZ;
  eye.up.x=upX;
  eye.up.y=upY;
  eye.up.z=upZ;

  // 计算眼睛各坐标轴的矢量
  VECTOR v1,v2,v3;
  VectorSub(v1,eye.center,eye.origin);
  VectorNormalize(v1);
  VectorCopy(v2,eye.up);
  VectorCross(v3,v2,v1);
  VectorNormalize(v3);
  VectorCross(v2,v3,v1);
  VectorCopy(eye.axisX,v3);
  VectorCopy(eye.axisY,v2);
  VectorCopy(eye.axisZ,v1);
}

// transform point P(x,y,z) from world coordinate to eye coordinate
void WorldToEye(float point[3], float result[3], EYE eye)
{
	float tmp[3][3];
	float a[3][1];
	float b[3][1];

	tmp[0][0]=(eye.axisX).x;
	tmp[1][0]=(eye.axisX).y;
	tmp[2][0]=(eye.axisX).z;
	tmp[0][1]=(eye.axisY).x;
	tmp[1][1]=(eye.axisY).y;
	tmp[2][1]=(eye.axisY).z;
	tmp[0][2]=(eye.axisZ).x;
	tmp[1][2]=(eye.axisZ).y;
	tmp[2][2]=(eye.axisZ).z;

	InvMatrixf(&tmp[0][0],3);
	a[0][0]=point[0];
	a[1][0]=point[1];
	a[2][0]=point[2];
	
	MultMatrixf( &b[0][0], &tmp[0][0], &a[0][0], 3, 3, 3, 1);
	result[0]=b[0][0]-eye.origin.x;
	result[1]=b[1][0]-eye.origin.y;
	result[2]=b[2][0]+eye.origin.z;
}

// transform point P(x,y,z) from eye coordinate to world coordinate
void EyeToWorld(float point[3], float result[3], EYE eye)
{
  VECTOR v1,v2,v3,v4,v5,v6;

  VectorCopy(v1,eye.axisZ);
  VectorCopy(v2,eye.axisY);
  VectorCopy(v3,eye.axisX);
  VectorScale(v3,point[0]);
  VectorScale(v2,point[1]);
  VectorScale(v1,point[2]);
  VectorAdd(v4,v1,v2);
  VectorAdd(v5,v4,v3);
  VectorAdd(v6,eye.origin,v5);
  result[0]=v6.x;
  result[1]=v6.y;
  result[2]=v6.z;
}

// rotate in eye coordinate
void EyeRotate(float point[3], float normal[3], float angle, 
				  EYE eye)
{
  float pp[3];
  float nn[3];
  float tt[3];

  float t[3]={0.0f,0.0f,0.0f};
  EyeToWorld(point, pp, eye);
  EyeToWorld(normal, nn, eye);
  EyeToWorld(t, tt, eye);
  nn[0]-=tt[0];
  nn[1]-=tt[1];
  nn[2]-=tt[2];
  WorldRotate(pp, nn, angle);
}

// translate in eye coordinate
void EyeTranslate(float p[3], EYE eye)
{
  float pp[3];
  float tt[3];

  float t[3]={0.0f,0.0f,0.0f};
  EyeToWorld(p, pp, eye);
  EyeToWorld(t, tt, eye);
  pp[0]-=tt[0];
  pp[1]-=tt[1];
  pp[2]-=tt[2];
  WorldTranslate(pp);
}