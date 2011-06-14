#ifndef INCLUDED_OFFSET_GEOMETRY_H
#define INCLUDED_OFFSET_GEOMETRY_H

#include <math.h>

#include <stdio.h>

#define LO 0
#define HI 1

#define XX 0
#define YY 1
#define ZZ 2

#define FALSE           0
#define TRUE            1

/*--------------------------------- Macros ----------------------------------*/

#define FMAX(x,y) ((x)>(y) ? (x) : (y))
#define FMIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define POINT_IN_BOX(p,box) \
	((p[X]<box[HI][XX])&&(p[YY]<box[HI][YY])&&(p[ZZ]<box[HI][ZZ])&&(p[XX]>=box[LO][XX])&&(p[YY]>=box[LO][YY])&&(p[ZZ]>=box[LO][ZZ]))

#define BBOX_OVERLAP(box1, box2)						\
    (((((box1)[HI][XX] < (box2)[LO][XX]) || ((box2)[HI][XX] < (box1)[LO][XX])) ||	\
     (((box1)[HI][YY] < (box2)[LO][YY]) || ((box2)[HI][YY] < (box1)[LO][YY])) ||	\
     (((box1)[HI][ZZ] < (box2)[LO][ZZ]) || ((box2)[HI][ZZ] < (box1)[LO][ZZ]))) ?	\
       FALSE : TRUE)


#define BBOX_OVERLAP(box1, box2)						\
    (((((box1)[HI][XX] < (box2)[LO][XX]) || ((box2)[HI][XX] < (box1)[LO][XX])) ||	\
     (((box1)[HI][YY] < (box2)[LO][YY]) || ((box2)[HI][YY] < (box1)[LO][YY])) ||	\
     (((box1)[HI][ZZ] < (box2)[LO][ZZ]) || ((box2)[HI][ZZ] < (box1)[LO][ZZ]))) ?	\
       FALSE : TRUE)


/*==================== 3D vector macros ===================*/

#define VEC3_ZERO(vec)	       { (vec)[0]=(vec)[1]=(vec)[2]=0; }
#define VEC4_ZERO(vec)	       { (vec)[0]=(vec)[1]=(vec)[2]=(vec)[3]=0; }
#define VEC3_NEG(dest,src)     { (dest)[0]= -(src)[0]; (dest)[1]= -(src)[1];(dest)[2]= -(src)[2];}
#define VEC4_NEG(dest,src)     { (dest)[0]= -(src)[0]; (dest)[1]= -(src)[1];(dest)[2]= -(src)[2]; (dest)[3]=-(src)[3];}
#define VEC3_EQ(a,b)           (((a)[0]==(b)[0]) && ((a)[1]==(b)[1]) && ((a)[2]==(b)[2]))
#define VEC4_EQ(a,b)           (((a)[0]==(b)[0]) && ((a)[1]==(b)[1]) && ((a)[2]==(b)[2])&&((a)[3]==(b)[3]))
#define ZERO3_TOL(a, tol)      { (a)[0] = (((a)[0]<tol)&&((a)[0]>-tol))?0.0:(a)[0];\
				 (a)[1] = (((a)[1]<tol)&&((a)[1]>-tol))?0.0:(a)[1];\
			         (a)[2] = (((a)[2]<tol)&&((a)[2]>-tol))?0.0:(a)[2];\
			       }

#define VEC3_V_OP_S(a,b,op,c)  {  (a)[0] = (b)[0] op (c);  \
				  (a)[1] = (b)[1] op (c);  \
				  (a)[2] = (b)[2] op (c);  }

#define VEC4_V_OP_S(a,b,op,c)  {  (a)[0] = (b)[0] op (c);  \
				  (a)[1] = (b)[1] op (c);  \
				  (a)[2] = (b)[2] op (c);  \
				  (a)[3] = (b)[3] op (c);  }

#define VEC3_V_OP_V(a,b,op,c)  { (a)[0] = (b)[0] op (c)[0]; \
				 (a)[1] = (b)[1] op (c)[1]; \
				 (a)[2] = (b)[2] op (c)[2]; \
				}

#define VEC4_V_OP_V(a,b,op,c)  { (a)[0] = (b)[0] op (c)[0]; \
				 (a)[1] = (b)[1] op (c)[1]; \
				 (a)[2] = (b)[2] op (c)[2]; \
				 (a)[3] = (b)[3] op (c)[3]; \
				}

#define VEC3_V_OP_V_OP_S(a,b,op1,c,op2,d)  \
				{ (a)[0] = (b)[0] op1 (c)[0] op2 (d); \
				  (a)[1] = (b)[1] op1 (c)[1] op2 (d); \
				  (a)[2] = (b)[2] op1 (c)[2] op2 (d); }

#define VEC3_VOPV_OP_S(a,b,op1,c,op2,d)  \
				{ (a)[0] = ((b)[0] op1 (c)[0]) op2 (d); \
				  (a)[1] = ((b)[1] op1 (c)[1]) op2 (d); \
				  (a)[2] = ((b)[2] op1 (c)[2]) op2 (d); }

#define VEC3_V_OP_V_OP_V(a,b,op1,c,op2,d)  \
				{ (a)[0] = (b)[0] op1 (c)[0] op2 (d)[0]; \
				  (a)[1] = (b)[1] op1 (c)[1] op2 (d)[1]; \
				  (a)[2] = (b)[2] op1 (c)[2] op2 (d)[2]; }

#define VEC3_ASN_OP(a,op,b)      {a[0] op b[0]; a[1] op b[1]; a[2] op b[2];}
#define VEC4_ASN_OP(a,op,b)      {a[0] op b[0]; a[1] op b[1]; a[2] op b[2]; a[3] op b[3];}

#define DOTPROD3(a, b)		 ((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2])
#define DOTPROD4(a, b)		 ((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2] + (a)[3]*(b)[3])

#define CROSSPROD3(a,b,c)       {(a)[0]=(b)[1]*(c)[2]-(b)[2]*(c)[1]; \
                                 (a)[1]=(b)[2]*(c)[0]-(b)[0]*(c)[2]; \
                                 (a)[2]=(b)[0]*(c)[1]-(b)[1]*(c)[0];}


#define SQ_DIST3(a, b)          (((a)[0]-(b)[0])*((a)[0]-(b)[0]) +      \
                                 ((a)[1]-(b)[1])*((a)[1]-(b)[1]) +      \
                                 ((a)[2]-(b)[2])*((a)[2]-(b)[2]))

/* assumes normalized plane normal */
#define POINT_PLANE_DIST(point, plane)  (DOTPROD3(point,plane)-plane[3])

/*==================== Transformation Macros===================*/
#define TRANSFORM_POINT(dest_point, matrix, src_point)		\
        {							\
            (dest_point)[XX] = (matrix)[0][0]*(src_point)[XX] +	\
		              (matrix)[0][1]*(src_point)[YY] +	\
			      (matrix)[0][2]*(src_point)[ZZ] +	\
			      (matrix)[0][3];			\
            (dest_point)[YY] = (matrix)[1][0]*(src_point)[XX] +	\
		              (matrix)[1][1]*(src_point)[YY] +	\
			      (matrix)[1][2]*(src_point)[ZZ] +	\
			      (matrix)[1][3];			\
            (dest_point)[ZZ] = (matrix)[2][0]*(src_point)[XX] +	\
		              (matrix)[2][1]*(src_point)[YY] +	\
			      (matrix)[2][2]*(src_point)[ZZ] +	\
			      (matrix)[2][3];			\
        }

#define TRANSFORM_VECTOR(dest_vector, matrix, src_vector)	\
        {							\
            (dest_vector)[XX] = (matrix)[0][0]*(src_vector)[XX] +	\
		               (matrix)[0][1]*(src_vector)[YY] +	\
			       (matrix)[0][2]*(src_vector)[ZZ];	\
            (dest_vector)[YY] = (matrix)[1][0]*(src_vector)[XX] +	\
		               (matrix)[1][1]*(src_vector)[YY] +	\
			       (matrix)[1][2]*(src_vector)[ZZ];	\
            (dest_vector)[ZZ] = (matrix)[2][0]*(src_vector)[XX] +	\
		               (matrix)[2][1]*(src_vector)[YY] +	\
			       (matrix)[2][2]*(src_vector)[ZZ];	\
        }




#define VERTS_TO_LINESEG(vert1, vert2, lineseg)				\
        {								\
	    VEC3_ASN_OP((lineseg).endpoints[0], =, (vert1)->coord);	\
	    VEC3_ASN_OP((lineseg).endpoints[1], =, (vert2)->coord);	\
	}

/*---------------------------------- Types ----------------------------------*/


typedef float Point[3];
typedef float Vector[3];
typedef float Matrix[3][4];
typedef float Extents[2][3];

typedef struct RawTriangle
{
    Point  verts[3];
    float plane_eq[4];
} RawTriangle;

typedef struct RawEdge
{
    Point  verts[2];
} RawEdge;

typedef struct LineSegment
{
    Point  endpoints[2];
} LineSegment;


/* Protection from multiple includes. */
#endif INCLUDED_OFFSET_GEOMETRY_H
