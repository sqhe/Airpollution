/*
* Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
*
* NVIDIA Corporation and its licensors retain all intellectual property and 
* proprietary rights in and to this software and related documentation. 
* Any use, reproduction, disclosure, or distribution of this software 
* and related documentation without an express license agreement from
* NVIDIA Corporation is strictly prohibited.
*
* Please refer to the applicable NVIDIA end user license agreement (EULA) 
* associated with this source code for terms and conditions that govern 
* your use of this NVIDIA software.
* 
*/

/*
CUDA particle system with volumetric shadows
sgreen 11/2008

This sample demonstrates a technique for rendering realistic volumetric
shadows through a cloud of particles. It uses CUDA for the simulation and
depth sorting of the particles, and OpenGL for rendering.

See the accompanying documentation for more details on the algorithm.

This file handles OpenGL initialization and the user interface.
*/

#include "stdafx.h"
#include "particleSystem.h"
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <math.h>
#include "ColorRamp.h"
#define M_PI		3.141592653589793238462643383280
#include <GL/glew.h>
#if defined(__APPLE__) || defined(__MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#if defined (_WIN32)
#include <GL/wglew.h>
#endif

#include <cutil_inline.h>
#include <cutil_gl_inline.h>
#include <cuda_gl_interop.h>
#include <rendercheck_gl.h>

#include "ParticleSystem.h"
#include "ParticleSystem.cuh"
#include "SmokeRenderer.h"
#include "paramgl.h"
#include "GLSLProgram.h"
#include "SmokeShaders.h"
#include "SmokeSystem.h"
//*****************************************************************************
#include "skymesh.h"
#include "cgtk.h"
//*****************************************************************************

#ifndef max_f
#define max_f(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min_f
#define min_f(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#define DTOR            0.0174532925
bool bStereo=true;
re_dat_tet *obj= NULL;
unsigned int numParticles = 1<<16;//1000;//64;//
ParticleSystem *psystem = 0;
SmokeRenderer *renderer = 0;
GLSLProgram *floorProg = 0;

//*******************************************************
GLSLProgram *skyProg = 0;
//GLSLProgram *bildProg = 0;
//*******************************************************


//for generating particles continuously
ParticleConfig beingInit=_NUM_CONFIGS;
unsigned int initCount=0;
unsigned int initBatchN=600;
unsigned int m_cur_step;
unsigned int m_batch_span=3;
int showPathN=100;
// view params
int ox, oy;
int buttonState = -1;

//*******************************************************
float    vv=4;
//*******************************************************

bool keyDown[256];
float activeArea[6];//bounding box, xmin,xmax,ymin,ymax,zmin,zmax
vec3f cameraPos(0, -2, -10);
vec3f cameraRot(0, 0, 0);
float focallength= 2.0;  /* Focal Length along vd   */
float aperture= 50;     /* Camera aperture         */
float eyesep = 0.08;//focallength / 20;/* Eye separation          */
vec3f cameraPosLag(cameraPos);
vec3f cameraRotLag(cameraRot);
vec3f cursorPos(0, 1, 0);
vec3f cursorPosLag(cursorPos);

//vec3f lightPos(0,16.0982*vv, 0);
//vec3f lightPos(-5,5, -5);
vec3f lightPos(-4*vv,5*vv, -4*vv);
//*******************************************************
//vec3f lightPos1(0,16.0982*vv, 0);
vec3f lightPos1(-4*vv,5*vv, -4*vv);
//vec3f lightPos1(-5,5, -5);
vec3f lightPos2(-5,5, -5);
//vec3f lightPos2(0,16.0982*vv, 0);
vec3f sunPos(0,10, 15);
float	 sunStrenth=4;
float	 sunPow=1;
vec3f sunColor(1.0, 1.0, 0.8);
//*******************************************************
vec3f sourcePos(8.4510336,0.056158927,1.1638986);//sphere source
//vec3f sourcePos(0,3,0);//sphere source
int num_bv=20;
int num_bp=1<<16;

int sourceId=1000;
float sourceRadius=0.05;
const float inertia = 0.1f;
const float translateSpeed = 0.002f;
const float cursorSpeed = 0.01f;
const float rotateSpeed = 0.2f;
const float walkSpeed = 0.05f;

int mode = 0;
int displayMode = (int) SmokeRenderer::VOLUMETRIC;

// QA AutoTest
bool g_bQAReadback = false;
bool g_bQAGLVerify = false;

// toggles
bool displayEnabled = true;
//bool paused = true;//false;
bool displaySliders = false;
bool displaySkybox=true;
//****************************************************************
bool displayTemprature=false;
//****************************************************************
bool wireframe = false;
bool animateEmitter = true;
bool emitterOn = false;//true;
bool sortt = true;
bool displayLightBuffer = false;
bool drawVectors = false;
bool doBlur = false;

float emitterVel = 0.0f;
uint emitterRate = 1000;
float emitterRadius = 0.25;
float emitterSpread = 0.0;
uint emitterIndex = 0;

// simulation parameters
float timestep = 0.01f;
float currentTime = 0.0f;
float spriteSize = 0.02f;//0.1;
float alpha = 0.1;//1.0;//
float shadowAlpha = 0.02;
float particleLifetime = 10;//numParticles / emitterRate;
vec3f lightColor(1.0, 1.0, 0.8);
vec3f colorAttenuation(0.5, 0.75, 1.0);
float blurRadius = 2.0;

int numSlices = 64;//16;//
int numDisplayedSlices = numSlices;

// fps
static int fpsCount = 0;
static int fpsLimit = 1;
unsigned int timer;

float modelView[16];
float cameraModelView[16];
ParamListGL *params;

GLuint floorTex = 0;

//************************************************************************************************
GLuint skyTex = 0;
GLuint skyTexfloor=0;
skymesh *sky_obj;
cgtk  *cgtk_obj;
//************************************************************************************************

// CheckRender object for verification
CheckRender *g_CheckRender = NULL;
int frameCheckNumber = 2;
int frameNumber = 0;
#define MAX_EPSILON_ERROR 10.0f
#define THRESHOLD          0.40f

// Define the files that are to be saved and the reference images for validation
const char *sSDKsample = "CUDA Smoke Particles";

const char *sOriginal  = "smokeParticles.ppm";
const char *sReference = "smokeParticles_ref.ppm";
const char *sRefBin[]  = 
{ 
	"ref_smokePart_pos.bin", 
	"ref_smokePart_vel.bin", 
	NULL 
};
void runEmitter();
void idle();

void setCgWindow(int w,int h)
{
	int a,b;
	a=0;
	b=0;
	a=cgtk_obj->WINDOW_SIZE_W-w;
	b=cgtk_obj->WINDOW_SIZE_H-h;

	if((a!=0)||(b!=0))
	{
		cgtk_obj->WINDOW_SIZE_W=w;
		cgtk_obj->WINDOW_SIZE_H=h;
		cgtk_obj->reset_fbo();
		//cgtk_obj->resize(w,h);
	}

}

void set3dtexture()
{
	float pminmax[2];

	pminmax[0]=-0.35;
	pminmax[1]=0.55;

	cgtk_obj->text_generate(obj,pminmax,true);
}

void setLight()
{
	vec4f lightPosP=vec4f(lightPos,1.0);
	vec4f lightColorP=vec4f(lightColor,1.0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightColorP);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColorP);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPosP);
	glShadeModel (GL_SMOOTH);
	//glDisable (GL_LINE_SMOOTH);

	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// 	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, lightColor);
	// 	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, lightColor);
	// 	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,lightColor);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE); 
	glEnable(GL_COLOR_MATERIAL);

}

void setsunpowertransfer()
{
	if(displayTemprature)
	{
		sunPow=0.4;
		lightColor=vec3f(0.5, 0.5, 0.4);
	}
	else
	{
		sunPow=1;
		lightColor=vec3f(1.0, 1.0, 0.8);
	}

}

void drawSphereSource()
{

	glEnable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(sourcePos.x,sourcePos.y,sourcePos.z);

	glColor3f(1,1,0);
	glutWireSphere(sourceRadius, 10, 10);
	glPopMatrix();
	//glDisable(GL_LIGHTING);
}
void drawAciveArea()
{
	if (obj == NULL)
		return;
	//glDisable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);
	glVertex3f(activeArea[0], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[5]);
	glVertex3f(activeArea[0], activeArea[2], activeArea[5]);

	glVertex3f(activeArea[0], activeArea[2], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[5]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[5]);


	glVertex3f(activeArea[0], activeArea[3], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[4]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[4]);


	glVertex3f(activeArea[0], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[4]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[4]);

	glVertex3f(activeArea[0], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[4]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[5]);
	glVertex3f(activeArea[0], activeArea[2], activeArea[5]);

	glVertex3f(activeArea[1], activeArea[2], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[5]);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glEnable(GL_BLEND);
	glColor4f(0.5f, 0.5f, 0.0f,0.5f);
	glLineWidth(5.f);

	glBegin(GL_QUADS);
	glVertex3f(activeArea[0], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[5]);
	glVertex3f(activeArea[0], activeArea[2], activeArea[5]);

	glVertex3f(activeArea[0], activeArea[2], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[5]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[5]);


	glVertex3f(activeArea[0], activeArea[3], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[4]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[4]);


	glVertex3f(activeArea[0], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[4]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[4]);

	glVertex3f(activeArea[0], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[4]);
	glVertex3f(activeArea[0], activeArea[3], activeArea[5]);
	glVertex3f(activeArea[0], activeArea[2], activeArea[5]);

	glVertex3f(activeArea[1], activeArea[2], activeArea[5]);
	glVertex3f(activeArea[1], activeArea[2], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[4]);
	glVertex3f(activeArea[1], activeArea[3], activeArea[5]);
	glEnd();

	glLineWidth(1.f);
	glPopMatrix();
	//glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}
void setAirData()
{
	psystem->setAirData();
	psystem->reset(CONFIG_AIR_RANDOM,0,numParticles,particleLifetime);

	cgtk_obj->setb(obj->boud_box);

}
bool resetPaticles(ParticleConfig config,unsigned int beginIndex,unsigned int num)
{
	return psystem->reset(config,beginIndex,num,particleLifetime);
}
bool initparticlesInShpere(unsigned int beginIndex,unsigned int num)
{
	obj->find_tet_in_sphere(sourcePos.x,sourcePos.y,sourcePos.z,sourceRadius);
	return resetPaticles(CONFIG_INLET_RANDOM,beginIndex,num);
}

bool initparticlesInRandomVolumn(unsigned int beginIndex,unsigned int num)
{
	obj->find_tet_in_bvolumn(num_bv);
	return resetPaticles(CONFIG_INLET_RANDOM,beginIndex,num);
}

bool initparticlesInRandomPlain(unsigned int beginIndex,unsigned int num)
{
	obj->find_tet_in_bplain(num_bp);
	return resetPaticles(CONFIG_INLET_RANDOM,beginIndex,num);
}
// initialize particle system
void initParticles(int numParticles, bool bUseVBO, bool bUseGL)
{
	psystem = new ParticleSystem(numParticles, bUseVBO, bUseGL);
	psystem->reset(CONFIG_RANDOM,0,numParticles,particleLifetime);
	if (bUseVBO) {
		renderer = new SmokeRenderer(numParticles);
		renderer->setLightTarget(vec3f(0.0, 1.0, 0.0));

		cutilCheckError(cutCreateTimer(&timer));
	}
}
void show_tet(int tetId,int4* tet_vers,float4 *vers)
{
	int i=tetId;
	if (i<0)
	{return;
	}
	//glPolygonMode(GL_FRONT,GL_LINE);
	int indices0, indices1, indices2;
	glBegin(GL_TRIANGLES);
	indices0=tet_vers[i].x;
	indices1=tet_vers[i].y;
	indices2=tet_vers[i].z;
	glVertex3f(vers[indices0].x,vers[indices0].y,vers[indices0].z);
	glVertex3f(vers[indices1].x,vers[indices1].y,vers[indices1].z);
	glVertex3f(vers[indices2].x,vers[indices2].y,vers[indices2].z);
	glEnd();
	glBegin(GL_TRIANGLES);
	indices0=tet_vers[i].y;
	indices1=tet_vers[i].w;
	indices2=tet_vers[i].z;
	glVertex3f(vers[indices0].x,vers[indices0].y,vers[indices0].z);
	glVertex3f(vers[indices1].x,vers[indices1].y,vers[indices1].z);
	glVertex3f(vers[indices2].x,vers[indices2].y,vers[indices2].z);
	glEnd();

	glBegin(GL_TRIANGLES);
	indices0=tet_vers[i].x;
	indices1=tet_vers[i].z;
	indices2=tet_vers[i].w;		
	glVertex3f(vers[indices0].x,vers[indices0].y,vers[indices0].z);		
	glVertex3f(vers[indices1].x,vers[indices1].y,vers[indices1].z);		
	glVertex3f(vers[indices2].x,vers[indices2].y,vers[indices2].z);
	glEnd();

	glBegin(GL_TRIANGLES);
	indices0=tet_vers[i].x;
	indices1=tet_vers[i].w;
	indices2=tet_vers[i].y;		
	glVertex3f(vers[indices0].x,vers[indices0].y,vers[indices0].z);
	glVertex3f(vers[indices1].x,vers[indices1].y,vers[indices1].z);		
	glVertex3f(vers[indices2].x,vers[indices2].y,vers[indices2].z);
	glEnd();
}
void debug_show_tet_with_particles(int1 *tetIds,int4* tet_vers,int4* neighbors,float4 *vers,int num,float4 *pos,float4 *vel,float4 *intersetions)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	int i,j;
	int4 neighborId;
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	for (int j =0;j<num;++j)
	{

		i=tetIds[j].x;
		if (i<0)
		{
			continue;
		}
		neighborId=neighbors[i];
		glColor3f(1.f, 0.f, 0.f);
		show_tet(i,tet_vers,vers);
		// 		glColor3f(1.f, 1.f, 1.f);
		// 		show_tet(neighborId.x,tet_vers,vers);
		// 		show_tet(neighborId.y,tet_vers,vers);
		// 		show_tet(neighborId.z,tet_vers,vers);
		// 		show_tet(neighborId.w,tet_vers,vers);
	}
	//draw particles
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	float temp_factor;
	for (int j=0;j<num;++j)
	{
		// 		glColor3f(1.f, 0.f, 0.f);
		// 		glPushMatrix();
		// 		glTranslatef(pos[j].x, pos[j].y, pos[j].z);
		// 		glutSolidSphere(0.03, 10, 5);
		// 		glPopMatrix();
		glColor3f(0.f, 0.f, 1.f);
		glPushMatrix();
		glTranslatef(intersetions[j].x, intersetions[j].y, intersetions[j].z);
		glutSolidSphere(0.03, 10, 5);
		glPopMatrix();
		glColor3f(1.f, 1.f, 1.f);
		glBegin(GL_LINES);
		glVertex3f(pos[j].x, pos[j].y, pos[j].z);
		temp_factor=2;//(intersetions[j].x-pos[j].x)/vel[j].x;
		glVertex3f(temp_factor*vel[j].x+pos[j].x,temp_factor*vel[j].y+pos[j].y,temp_factor*vel[j].z+pos[j].z);
		glEnd();
	}
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glPopMatrix();
};
void drawPathes()
{
	CColorRamp colormap;
	colormap.BuildRainbow();
	float red, green, blue;
	float3 temp_start,temp_end,temp_vec;
	float vec_length;
	std::vector<float3>* particle_path=psystem->getPathes();
	std::vector<float>* path_vel=psystem->getPathVel();
	GLUquadricObj *quadratic = gluNewQuadric();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glColor4f(0.2f,0.368f,0.3686f,0.5);
	glLineWidth(3.0f);
	//glEnable(GL_LIGHTING);
	int path_length;
	int pathN=showPathN;
	pathN=min_1(initCount,pathN);
	for (int i=0;i<pathN;++i)
	{
		/*path_length=particle_path[i].size()-1;
		for (int j=0;j<path_length;++j)
		{
		float texcoord = (path_vel[i][j] - psystem->getPathVelMin()) / (psystem->getPathVelMax() - psystem->getPathVelMin()) * 255;
		red = min_f( max_f((float)colormap.Red(texcoord)/255.f, 0.f), 1.f );
		green = min_f( max_f((float)colormap.Green(texcoord)/255.f, 0.f), 1.f );
		blue = min_f( max_f((float)colormap.Blue(texcoord)/255.f, 0.f), 1.f );
		glColor3f( red, green, blue );
		temp_start=particle_path[i][j];
		temp_end=particle_path[i][j+1];
		temp_vec.x=temp_end.x-temp_start.x;
		temp_vec.y=temp_end.y-temp_start.y;
		temp_vec.z=temp_end.z-temp_start.z;
		vec_length=sqrt(temp_vec.x*temp_vec.x+temp_vec.y*temp_vec.y+temp_vec.z*temp_vec.z);
		if (vec_length<1e-5)
		continue;
		glPushMatrix();
		double angle = acos(temp_vec.z/vec_length) / (2*M_PI) * 360;
		double rotx = -temp_vec.y/vec_length;
		double roty = temp_vec.x/vec_length;
		double rotz = 0;
		rotx = rotx / sqrt(rotx*rotx + roty*roty);
		roty = roty / sqrt(rotx*rotx + roty*roty);
		glTranslatef(temp_start.x,temp_start.y,temp_start.z);
		glRotatef(angle, rotx, roty, rotz);
		gluCylinder(quadratic,0.02f,0.02f,vec_length,6,1);
		glPopMatrix();
		}*/

		float3 temp;
		glBegin(GL_LINE_STRIP);
		for (int j=0;j<particle_path[i].size();++j)
		{
			float texcoord = (path_vel[i][j] - psystem->getPathVelMin()) / (psystem->getPathVelMax() - psystem->getPathVelMin()) * 255;
			red = min_f( max_f((float)colormap.Red(texcoord)/255.f, 0.f), 1.f );
			green = min_f( max_f((float)colormap.Green(texcoord)/255.f, 0.f), 1.f );
			blue = min_f( max_f((float)colormap.Blue(texcoord)/255.f, 0.f), 1.f );
			glColor3f( red, green, blue );
			temp=particle_path[i][j];
			glVertex3f(temp.x,temp.y,temp.z);
		}
		glEnd();

	}

	glPopMatrix();
	glLineWidth(1.0f);
	//glDisable(GL_LIGHTING);
	gluDeleteQuadric(quadratic);
}
void cleanup()
{
	if (psystem)   delete psystem;
	if (renderer)  delete renderer;
	if (floorProg) delete floorProg;
	cutilCheckError(cutDeleteTimer(timer));
	if (params)    delete params;
	if (floorTex)  glDeleteTextures(1, &floorTex);

	//****************************************************************************************
	if (skyTex)  glDeleteTextures(1, &skyTex);
	if (skyTexfloor) glDeleteTextures(1,&skyTexfloor);
	if (sky_obj)	delete sky_obj;
	if (cgtk_obj)	delete cgtk_obj;
	if (skyProg)	delete skyProg;
	//if (bildProg)	delete bildProg;
	//****************************************************************************************

	if (g_CheckRender) {
		delete g_CheckRender; g_CheckRender = NULL;
	}
}

void drawfloor()
{
	// draw floor
	float vv=1;

	floorProg->enable();
	floorProg->bindTexture("tex", floorTex, GL_TEXTURE_2D, 0);
	floorProg->bindTexture("shadowTex", renderer->getShadowTexture(), GL_TEXTURE_2D, 1);
	floorProg->setUniformfv("lightPosEye", renderer->getLightPositionEyeSpace(), 3);
	floorProg->setUniformfv("lightColor", lightColor, 3);
	floorProg->setUniform1i("hasTexCoord",1);
	// set shadow matrix as texture matrix
	matrix4f shadowMatrix = renderer->getShadowMatrix();
	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf((GLfloat *) shadowMatrix.get_value());

	//float vv=0.1;

	glColor3f(1.0, 1.0, 1.0);
	glNormal3f(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	{
		float s = 20.f;
		float rep = 20.f;
		glTexCoord2f(0.f, 0.f); glVertex3f(-s*vv, 0*vv, -s*vv);
		glTexCoord2f(rep, 0.f); glVertex3f(s*vv, 0*vv, -s*vv);
		glTexCoord2f(rep, rep); glVertex3f(s*vv, 0*vv, s*vv);
		glTexCoord2f(0.f, rep); glVertex3f(-s*vv, 0*vv, s*vv);

	}
	glEnd();

	floorProg->setUniform1i("hasTexCoord",0);

	DrawBuilding();
	DrawBounding();
	floorProg->disable();
	//DrawBuilding();
}

void drawsky()
{
	skyProg->enable();
	skyProg->bindTexture("tex", skyTex, GL_TEXTURE_2D, 0);
	skyProg->bindTexture("shadowTex", renderer->getShadowTexture(), GL_TEXTURE_2D, 1);
	skyProg->setUniformfv("sunPos", sunPos, 3);
	skyProg->setUniformfv("sunColor", sunColor, 3);
	skyProg->setUniform1f("sunStrenth", sunStrenth);
	skyProg->setUniform1f("sunShiness", 512);
	skyProg->setUniform1f("sunPow", sunPow);
	skyProg->setUniform1i("hasTexCoord",1);
	// set shadow matrix as texture matrix
	matrix4f shadowMatrix = renderer->getShadowMatrix();
	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf((GLfloat *) shadowMatrix.get_value());

	glColor3f(1.0, 1.0, 1.0);
	glNormal3f(0.0, 1.0, 0.0);

	sky_obj->draw(vv);

	skyProg->disable();

	// draw floor
	floorProg->enable();
	floorProg->bindTexture("tex", skyTexfloor, GL_TEXTURE_2D, 0);
	floorProg->bindTexture("shadowTex", renderer->getShadowTexture(), GL_TEXTURE_2D, 1);
	floorProg->setUniformfv("lightPosEye", renderer->getLightPositionEyeSpace(), 3);
	floorProg->setUniformfv("lightColor", lightColor, 3);
	floorProg->setUniform1i("hasTexCoord",1);
	// set shadow matrix as texture matrix
	shadowMatrix = renderer->getShadowMatrix();
	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf((GLfloat *) shadowMatrix.get_value());

	//float vv=0.1;

	glColor3f(1.0, 1.0, 1.0);
	glNormal3f(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	{
		float s = 20.f;
		float rep = 20.f;
		glTexCoord2f(0.f, 0.f); glVertex3f(-s*vv, 0*vv, -s*vv);
		glTexCoord2f(rep, 0.f); glVertex3f(s*vv, 0*vv, -s*vv);
		glTexCoord2f(rep, rep); glVertex3f(s*vv, 0*vv, s*vv);
		glTexCoord2f(0.f, rep); glVertex3f(-s*vv, 0*vv, s*vv);

	}
	glEnd();
	//sky_obj->draw(vv);
	floorProg->setUniform1i("hasTexCoord",0);

	DrawBuilding();
	DrawBounding();
	floorProg->disable();
	//DrawBuilding();

}

void drawbackground(bool s)
{
	if(s)
		drawsky();
	else
		drawfloor();
}

void renderScene()
{
	//   glEnable(GL_DEPTH_TEST);
	//   glDepthMask(GL_TRUE);

	//drawbackground(displaySkybox);

	////   // draw floor
	////floorProg->enable();
	////floorProg->bindTexture("tex", floorTex, GL_TEXTURE_2D, 0);
	////floorProg->bindTexture("shadowTex", renderer->getShadowTexture(), GL_TEXTURE_2D, 1);
	////floorProg->setUniformfv("lightPosEye", renderer->getLightPositionEyeSpace(), 3);
	////floorProg->setUniformfv("lightColor", lightColor, 3);
	////floorProg->setUniform1i("hasTexCoord",1);
	////// set shadow matrix as texture matrix
	////matrix4f shadowMatrix = renderer->getShadowMatrix();
	////   glActiveTexture(GL_TEXTURE0);
	////   glMatrixMode(GL_TEXTURE);
	////   glLoadMatrixf((GLfloat *) shadowMatrix.get_value());

	////glColor3f(1.0, 1.0, 1.0);
	////glNormal3f(0.0, 1.0, 0.0);
	////   glBegin(GL_QUADS);
	////   {
	////       float s = 20.f;
	////       float rep = 20.f;
	////       glTexCoord2f(0.f, 0.f); glVertex3f(-s, 0, -s);
	////       glTexCoord2f(rep, 0.f); glVertex3f(s, 0, -s);
	////       glTexCoord2f(rep, rep); glVertex3f(s, 0, s);
	////       glTexCoord2f(0.f, rep); glVertex3f(-s, 0, s);
	////   }
	////   glEnd();
	////floorProg->setUniform1i("hasTexCoord",0);
	////DrawBuilding();
	////   floorProg->disable();


	///*    // draw building
	//bildProg->enable();
	////bildProg->bindTexture("tex", floorTex, GL_TEXTURE_2D, 0);
	////bildProg->bindTexture("shadowTex", renderer->getShadowTexture(), GL_TEXTURE_2D, 1);
	//bildProg->setUniformfv("lightPosEye", renderer->getLightPositionEyeSpace(), 3);
	//bildProg->setUniformfv("lightColor", lightColor, 3);
	//bildProg->setUniform1i("hasTexCoord",1);
	//// set shadow matrix as texture matrix
	//matrix4f shadowMatrix = renderer->getShadowMatrix();
	//   //glActiveTexture(GL_TEXTURE0);
	//   //glMatrixMode(GL_TEXTURE);
	//   glLoadMatrixf((GLfloat *) shadowMatrix.get_value());

	//glColor3f(1.0, 1.0, 1.0);
	//glNormal3f(0.0, 1.0, 0.0);

	//bildProg->setUniform1i("hasTexCoord",0);
	//DrawBuilding();
	//   bildProg->disable();*/

	//DrawAirData();
	////if (psystem->hasSetAirData())
	////	debug_show_tet_with_particles(psystem->getVerTetIds(),psystem->getTets(),psystem->getNeighbors(),psystem->getVers(),psystem->getNumParticles(),psystem->getPos(),psystem->getVel(),psystem->getIntersections());
	//   glMatrixMode(GL_TEXTURE);
	//glLoadIdentity();

	//// draw light
	//   glMatrixMode(GL_MODELVIEW);
	//  /* glPushMatrix();
	//   glTranslatef(lightPos.x, lightPos.y, lightPos.z);
	//   glColor3fv(&lightColor[0]);
	//   glutSolidSphere(0.1, 10, 5);
	//   glPopMatrix();*/

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	drawbackground(displaySkybox);

	DrawAirData();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	if(displayTemprature)
	{
		//glDisable(GL_LIGHTING);
		cgtk_obj->display();
		//glEnable(GL_LIGHTING);

		//DrawBuilding();
		//DrawBounding();
	}
}

// main rendering loop
void updatePaticles()
{

	if (emitterOn) {
		runEmitter();
	}
	SimParams &p = psystem->getParams();
	p.cursorPos = make_float3(cursorPosLag.x, cursorPosLag.y, cursorPosLag.z);

	if (initCount>0)
	{
		psystem->step(timestep,initCount,particleLifetime); 
	}
	if (beingInit!=_NUM_CONFIGS&&initCount<numParticles&&m_cur_step%m_batch_span==0)
	{
		int initNum=numParticles/initBatchN;
		if (initCount+initNum>=numParticles)
		{
			initNum=numParticles-initCount-1;
		}
		resetPaticles(beingInit,initCount,initNum);
		if (initCount==numParticles-1)
		{
			beingInit=_NUM_CONFIGS;
		}
	}
	currentTime += timestep;
}
void display()
{
	cutilCheckError(cutStartTimer(timer));  




	// update the simulation


	renderer->calcVectors();
	vec3f sortVector = renderer->getSortVector();

	psystem->setSortVector(make_float3(sortVector.x, sortVector.y, sortVector.z));
	psystem->setModelView(modelView);
	psystem->setSorting(sortt);
	psystem->depthSort();

	// render
	//     glClearColor(0.0, 0.0, 0.0, 1.0);
	//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
	renderScene();

	// draw particles
	if (displayEnabled)
	{
		// render scene to offscreen buffers to get correct occlusion
		renderer->beginSceneRender(SmokeRenderer::LIGHT_BUFFER);
		renderScene();
		renderer->endSceneRender(SmokeRenderer::LIGHT_BUFFER);

		renderer->beginSceneRender(SmokeRenderer::SCENE_BUFFER);
		renderScene();
		renderer->endSceneRender(SmokeRenderer::SCENE_BUFFER);

		renderer->setPositionBuffer(psystem->getPosBuffer());
		renderer->setVelocityBuffer(psystem->getVelBuffer());
		renderer->setIndexBuffer(psystem->getSortedIndexBuffer());

		renderer->setNumParticles(initCount);
		renderer->setParticleRadius(spriteSize);
		renderer->setDisplayLightBuffer(displayLightBuffer);
		renderer->setAlpha(alpha);
		renderer->setShadowAlpha(shadowAlpha);
		renderer->setLightPosition(lightPos);
		renderer->setColorAttenuation(colorAttenuation);
		renderer->setLightColor(lightColor);
		renderer->setNumSlices(numSlices);
		renderer->setNumDisplayedSlices(numDisplayedSlices);
		renderer->setBlurRadius(blurRadius);

		renderer->render();

		if (drawVectors) {
			renderer->debugVectors();
		}
	}

	// display sliders
	if (displaySliders) {
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO); // invert color
		glEnable(GL_BLEND);
		params->Render(0, 0);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	//    glutSwapBuffers();
	//    glutReportErrors();
	cutilCheckError(cutStopTimer(timer));  

	// readback for verification//sw/devrel/SDK10/Compute/projects/recursiveGaussian/recursiveGaussian.cpp
	if (g_CheckRender && g_CheckRender->IsQAReadback() && (++frameNumber >= frameCheckNumber)) {
		printf("> (Frame %d) Readback BackBuffer\n", frameNumber);
		g_CheckRender->readback( winWidth, winHeight );
		g_CheckRender->savePPM(sOriginal, true, NULL);
		bool passed = g_CheckRender->PPMvsPPM(sOriginal, sReference, MAX_EPSILON_ERROR, THRESHOLD);
		printf("Summary: %d errors!\n", passed ? 0 : 1);
		printf("%s\n", passed ? "PASSED" : "FAILED");
		cleanup();
		exit(0);
	}

	fpsCount++;
	// this displays the frame rate updated every second (independent of frame rate)
	//if (fpsCount >= fpsLimit) {
	//    char fps[256];
	//    float ifps = 1.f / (cutGetAverageTimerValue(timer) / 1000.f);
	//    sprintf(fps, "CUDA Smoke Particles (%d particles): %3.1f fps", numParticles, ifps);  
	//    glutSetWindowTitle(fps);
	//    fpsCount = 0; 
	//    fpsLimit = (ifps > 1.f) ? (int)ifps : 1;
	//    if (paused) fpsLimit = 0;
	//    cutilCheckError(cutResetTimer(timer));  
	//}


}

// GLUT callback functions

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float) w / (float) h, 0.01, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);

	renderer->setFOV(60.0);
	renderer->setWindowSize(w, h);
}

void mouse(int button, int state, int x, int y)
{
	int mods;

	if (state == GLUT_DOWN)
		buttonState = button;
	else if (state == GLUT_UP)

	{
		buttonState = -1;
		if(mode==M_MOVE_SOURCE)
			obj->find_tet_in_sphere(sourcePos.x,sourcePos.y,sourcePos.z,sourceRadius);
	}

	/*mods = glutGetModifiers();
	if (mods & GLUT_ACTIVE_SHIFT) {
	buttonState = 2;
	} else if (mods & GLUT_ACTIVE_CTRL) {
	buttonState = 3;
	}*/

	ox = x; oy = y;

	if (displaySliders) {
		if (params->Mouse(x, y, button, state)) {
			//glutPostRedisplay();
			return;
		}
	}

	//glutPostRedisplay();
}

// transfrom vector by matrix
void xform(vec3f &v, vec3f &r, float *m)
{
	r.x = v.x*m[0] + v.y*m[4] + v.z*m[8] + m[12];
	r.y = v.x*m[1] + v.y*m[5] + v.z*m[9] + m[13];
	r.z = v.x*m[2] + v.y*m[6] + v.z*m[10] + m[14];
}

// transform vector by transpose of matrix (assuming orthonormal)
void ixform(vec3f &v, vec3f &r, float *m)
{
	r.x = v.x*m[0] + v.y*m[1] + v.z*m[2];
	r.y = v.x*m[4] + v.y*m[5] + v.z*m[6];
	r.z = v.x*m[8] + v.y*m[9] + v.z*m[10];
}

void ixformPoint(vec3f &v, vec3f &r, float *m)
{
	vec3f x;
	x.x = v.x - m[12];
	x.y = v.y - m[13];
	x.z = v.z - m[14];
	ixform(x, r, m);
}
void setLeftView(){
	// view transform
	vec3f r;
	double dist,ratio,radians,scale,wd2,ndfl;
	double left,right,top,bottom,near1=0.1,far1=10000;

	/* Clip to avoid extreme stereo */
	// 	if (bStereo)
	// 		near1 = focallength / 5;

	/* Misc stuff */
	ratio  = winWidth / (double)winHeight;
	radians = DTOR * aperture / 2;
	wd2     = near1 * tan(radians);
	ndfl    = near1 / focallength;
	vec3f v = vec3f(0.0, 0.0, 1.0);
	vec3f vu = vec3f(0.0, 1.0, 0.0);
	vec3f vd;
	ixform(v, vd, cameraModelView);
	vd=normalize(vd);
	r=cross(vd,vu);
	r=normalize(r);
	r *= eyesep / 2.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	left  = - ratio * wd2 + 0.5 * eyesep * ndfl;
	right =   ratio * wd2 + 0.5 * eyesep * ndfl;
	top    =   wd2;
	bottom = - wd2;
	glFrustum(left,right,bottom,top,near1,far1);

	glMatrixMode(GL_MODELVIEW);
	if (bStereo)	
		glDrawBuffer(GL_BACK_LEFT);
	else
		glDrawBuffer(GL_BACK);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRotatef(cameraRotLag[0], 1.0, 0.0, 0.0);
	glRotatef(cameraRotLag[1], 0.0, 1.0, 0.0);
	glTranslatef(cameraPosLag[0]- r.x, cameraPosLag[1]- r.y, cameraPosLag[2]- r.z);
	// 	gluLookAt(cameraPosLag.x - r.x,cameraPosLag.y - r.y,cameraPosLag.z - r.z,
	// 		cameraPosLag.x - r.x + vd.x,
	// 		cameraPosLag.y - r.y + vd.y,
	// 		cameraPosLag.z - r.z + vd.z,
	// 		vu.x,vu.y,vu.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
}
void setRightView(){
	// view transform
	vec3f r;
	double dist,ratio,radians,scale,wd2,ndfl;
	double left,right,top,bottom,near1=0.1,far1=10000;

	/* Clip to avoid extreme stereo */
	// 	if (bStereo)
	// 		near1 = focallength / 5;

	/* Misc stuff */
	ratio  = winWidth / (double)winHeight;
	radians = DTOR * aperture / 2;
	wd2     = near1 * tan(radians);
	ndfl    = near1 / focallength;
	vec3f v = vec3f(0.0, 0.0, 1.0);
	vec3f vu = vec3f(0.0, 1.0, 0.0);
	vec3f vd;
	ixform(v, vd, cameraModelView);
	vd=normalize(vd);
	r=cross(vd,vu);
	r=normalize(r);
	r *= eyesep / 2.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	left  = - ratio * wd2 - 0.5 * eyesep * ndfl;
	right =   ratio * wd2 - 0.5 * eyesep * ndfl;
	top    =   wd2;
	bottom = - wd2;
	glFrustum(left,right,bottom,top,near1,far1);

	glMatrixMode(GL_MODELVIEW);
	glDrawBuffer(GL_BACK_RIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRotatef(cameraRotLag[0], 1.0, 0.0, 0.0);
	glRotatef(cameraRotLag[1], 0.0, 1.0, 0.0);
	glTranslatef(cameraPosLag[0]+ r.x, cameraPosLag[1]+ r.y, cameraPosLag[2]+ r.z);
	// 	gluLookAt(cameraPosLag.x + r.x,cameraPosLag.y + r.y,cameraPosLag.z + r.z,
	// 		cameraPosLag.x + r.x + vd.x,
	// 		cameraPosLag.y + r.y + vd.y,
	// 		cameraPosLag.z + r.z + vd.z,
	// 		vu.x,vu.y,vu.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
}
void motion(int x, int y)
{
	float dx, dy;
	dx = x - ox;
	dy = y - oy;

	if (displaySliders&&buttonState != -1) {
		if (params->Motion(x, y)) {
			ox = x; oy = y;
			//glutPostRedisplay();
			return;
		}
	}

	switch(mode) 
	{
	case M_VIEW:
		{
			if (buttonState == GLUT_LEFT_BUTTON) {
				// left = rotate
				cameraRot[0] += dy * rotateSpeed;
				cameraRot[1] += dx * rotateSpeed;
			}
			if (buttonState == GLUT_RIGHT_BUTTON) {
				// middle = translate
				vec3f v = vec3f(dx*translateSpeed, dy*translateSpeed, 0.0f);
				vec3f r;
				ixform(v, r, modelView);
				cameraPos += r;
			}
			if (buttonState == GLUT_MIDDLE_BUTTON) {
				// left+middle = zoom
				vec3f v = vec3f(0.0, 0.0, dy*translateSpeed);
				vec3f r;
				ixform(v, r, modelView);
				cameraPos += r;
			} 
		}
		break;

	case M_MOVE_CURSOR:
		if (buttonState == GLUT_LEFT_BUTTON) {
			// left = rotate
			cameraRot[0] += dy * rotateSpeed;
			cameraRot[1] += dx * rotateSpeed;
		}
		if (buttonState==GLUT_MIDDLE_BUTTON) {
			focallength+=dy*cursorSpeed;
			if (focallength<0)
			{
				focallength=0;
			}
		}
		break;

	case M_MOVE_LIGHT:
		if (buttonState==GLUT_LEFT_BUTTON) {
			vec3f v = vec3f(dx*cursorSpeed, -dy*cursorSpeed, 0.0f);
			vec3f r;
			ixform(v, r, modelView);
			lightPos += r;
		} else if (buttonState==GLUT_MIDDLE_BUTTON) {
			vec3f v = vec3f(0.0f, 0.0f, dy*cursorSpeed);
			vec3f r;
			ixform(v, r, modelView);
			lightPos += r;
		}
		setLight();
		break;
	case M_MOVE_SOURCE:
		if (buttonState==GLUT_LEFT_BUTTON) {
			vec3f v = vec3f(dx*cursorSpeed, -dy*cursorSpeed, 0.0f);
			vec3f r;
			ixform(v, r, modelView);
			sourcePos += r;
			if(beingInit==CONFIG_INLET_RANDOM)
				obj->find_tet_in_sphere(sourcePos.x,sourcePos.y,sourcePos.z,sourceRadius);
		} else if (buttonState==GLUT_MIDDLE_BUTTON) {
			vec3f v = vec3f(0.0f, 0.0f, dy*cursorSpeed);
			vec3f r;
			ixform(v, r, modelView);
			sourcePos += r;
			if(beingInit==CONFIG_INLET_RANDOM)
				obj->find_tet_in_sphere(sourcePos.x,sourcePos.y,sourcePos.z,sourceRadius);
		}
		break;
	case M_MOVE_ACTIVEAREA:
		{vec3f activeAreaCenter(activeArea[0]+activeArea[1],activeArea[2]+activeArea[3],activeArea[4]+activeArea[5]);
		activeAreaCenter/=2;
		vec3f activeAreaHalfW(activeArea[1]-activeArea[0],activeArea[3]-activeArea[2],activeArea[5]-activeArea[4]);
		activeAreaHalfW/=2;
		if (buttonState==GLUT_LEFT_BUTTON) {
			// 			vec3f v = vec3f(dx*cursorSpeed, -dy*cursorSpeed, 0.0f);
			// 			vec3f r;
			// 			ixform(v, r, modelView);
			// 			activeAreaCenter += r;
			activeAreaCenter.x+=dy*cursorSpeed;
			activeArea[0]=activeAreaCenter.x-activeAreaHalfW.x;
			activeArea[1]=activeAreaCenter.x+activeAreaHalfW.x;
			activeArea[2]=activeAreaCenter.y-activeAreaHalfW.y;
			activeArea[3]=activeAreaCenter.y+activeAreaHalfW.y;
			activeArea[4]=activeAreaCenter.z-activeAreaHalfW.z;
			activeArea[5]=activeAreaCenter.z+activeAreaHalfW.z;
			setActiveArea(activeArea);
		} else if (buttonState==GLUT_MIDDLE_BUTTON) {
			// 			vec3f v = vec3f(0.0f, 0.0f, dy*cursorSpeed);
			// 			vec3f r;
			// 			ixform(v, r, modelView);
			// 			activeAreaCenter += r;
			activeAreaCenter.y+=dy*cursorSpeed;
			activeArea[0]=activeAreaCenter.x-activeAreaHalfW.x;
			activeArea[1]=activeAreaCenter.x+activeAreaHalfW.x;
			activeArea[2]=activeAreaCenter.y-activeAreaHalfW.y;
			activeArea[3]=activeAreaCenter.y+activeAreaHalfW.y;
			activeArea[4]=activeAreaCenter.z-activeAreaHalfW.z;
			activeArea[5]=activeAreaCenter.z+activeAreaHalfW.z;
			setActiveArea(activeArea);
		}
		else if (buttonState==GLUT_RIGHT_BUTTON) {
			// 			vec3f v = vec3f(0.0f, 0.0f, dy*cursorSpeed);
			// 			vec3f r;
			// 			ixform(v, r, modelView);
			// 			activeAreaCenter += r;
			activeAreaCenter.z+=dy*cursorSpeed;
			activeArea[0]=activeAreaCenter.x-activeAreaHalfW.x;
			activeArea[1]=activeAreaCenter.x+activeAreaHalfW.x;
			activeArea[2]=activeAreaCenter.y-activeAreaHalfW.y;
			activeArea[3]=activeAreaCenter.y+activeAreaHalfW.y;
			activeArea[4]=activeAreaCenter.z-activeAreaHalfW.z;
			activeArea[5]=activeAreaCenter.z+activeAreaHalfW.z;
			setActiveArea(activeArea);
		}
		break;
		}
	case M_SCALE_ACTIVEAREA:
		{vec3f activeAreaCenter(activeArea[0]+activeArea[1],activeArea[2]+activeArea[3],activeArea[4]+activeArea[5]);
		activeAreaCenter/=2;
		vec3f activeAreaHalfW(activeArea[1]-activeArea[0],activeArea[3]-activeArea[2],activeArea[5]-activeArea[4]);
		activeAreaHalfW/=2;
		if (buttonState==GLUT_LEFT_BUTTON) {
			activeAreaHalfW.x+=dy*cursorSpeed;
			activeArea[0]=activeAreaCenter.x-activeAreaHalfW.x;
			activeArea[1]=activeAreaCenter.x+activeAreaHalfW.x;
			activeArea[2]=activeAreaCenter.y-activeAreaHalfW.y;
			activeArea[3]=activeAreaCenter.y+activeAreaHalfW.y;
			activeArea[4]=activeAreaCenter.z-activeAreaHalfW.z;
			activeArea[5]=activeAreaCenter.z+activeAreaHalfW.z;
			setActiveArea(activeArea);
		}else if (buttonState==GLUT_MIDDLE_BUTTON) {
			activeAreaHalfW.y+=dy*cursorSpeed;
			activeArea[0]=activeAreaCenter.x-activeAreaHalfW.x;
			activeArea[1]=activeAreaCenter.x+activeAreaHalfW.x;
			activeArea[2]=activeAreaCenter.y-activeAreaHalfW.y;
			activeArea[3]=activeAreaCenter.y+activeAreaHalfW.y;
			activeArea[4]=activeAreaCenter.z-activeAreaHalfW.z;
			activeArea[5]=activeAreaCenter.z+activeAreaHalfW.z;
			setActiveArea(activeArea);
		}
		else if (buttonState==GLUT_RIGHT_BUTTON) {
			activeAreaHalfW.z+=dy*cursorSpeed;
			activeArea[0]=activeAreaCenter.x-activeAreaHalfW.x;
			activeArea[1]=activeAreaCenter.x+activeAreaHalfW.x;
			activeArea[2]=activeAreaCenter.y-activeAreaHalfW.y;
			activeArea[3]=activeAreaCenter.y+activeAreaHalfW.y;
			activeArea[4]=activeAreaCenter.z-activeAreaHalfW.z;
			activeArea[5]=activeAreaCenter.z+activeAreaHalfW.z;
			setActiveArea(activeArea);
		}
		break;
		}
	}


	ox = x; oy = y;
	//glutPostRedisplay();
}

void movelight(bool s)
{
	if(s)
		lightPos=lightPos1;
	else
		lightPos=lightPos2;

	setLight();
}

// commented out to remove unused parameter warnings in Linux
void key(unsigned char key, int /*x*/, int /*y*/)
{
	switch (key) 
	{
		//     case ' ':
		//         paused = !paused;
		//         break;
		//     case 13:
		//         psystem->step(timestep); 
		//         renderer->setPositionBuffer(psystem->getPosBuffer());
		//         renderer->setVelocityBuffer(psystem->getVelBuffer());
		//         break;
	case '\033':
		cleanup();
		exit(0);
		break;
	case 'v':
		mode = M_VIEW;
		animateEmitter = true;
		break;
	case 'm':
		mode = M_MOVE_CURSOR;
		break;
	case 'l':
		mode = M_MOVE_LIGHT;
		break;

	case 'r':
		displayEnabled = !displayEnabled;
		break;

	case '1':
		psystem->reset(CONFIG_AIR_RANDOM,0,numParticles,particleLifetime);
		break;

	case '2':
		emitterOn ^= 1;
		break;

	case 'W':
		wireframe = !wireframe;
		break;

	case 'g':
		mode=M_MOVE_SOURCE;
		break;
	case 'h':
		displaySliders = !displaySliders;
		break;

	case 'o':
		sortt ^= 1;
		psystem->setSorting(sortt);
		break;

	case 'D':
		displayLightBuffer ^= 1;
		break;

	case 'p':
		displayMode = (displayMode + 1) % SmokeRenderer::NUM_MODES;
		renderer->setDisplayMode((SmokeRenderer::DisplayMode) displayMode);
		break;
	case 'P':
		displayMode--;
		if (displayMode < 0) displayMode = SmokeRenderer::NUM_MODES - 1;
		renderer->setDisplayMode((SmokeRenderer::DisplayMode) displayMode);
		break;

	case 'V':
		drawVectors ^= 1;
		break;

	case '=':
		numSlices *= 2;
		if (numSlices > 256) numSlices = 256;
		numDisplayedSlices = numSlices;
		break;
	case '-':
		if (numSlices > 1) numSlices /= 2;
		numDisplayedSlices = numSlices;
		break;

	case 'b':
		doBlur ^= 1;
		renderer->setDoBlur(doBlur);
		break;

	case 'n':
		updatePaticles();
		break;
	}

	printf("numSlices = %d\n", numSlices);
	keyDown[key] = true;

	//glutPostRedisplay();
}

void keyUp(unsigned char key, int /*x*/, int /*y*/)
{
	keyDown[key] = false;
}

void runEmitter()
{
	vec3f vel = vec3f(0, emitterVel, 0);
	vec3f vx(1, 0, 0);
	vec3f vy(0, 0, 1);
	vec3f spread(emitterSpread, 0.0f, emitterSpread);

	psystem->sphereEmitter(emitterIndex, cursorPosLag, vel, spread, emitterRadius, (int) emitterRate*timestep, particleLifetime, particleLifetime*0.1);

	if (emitterIndex > numParticles-1)
		emitterIndex = 0;
}

void special(int k, int x, int y)
{
	if (displaySliders) {
		params->Special(k, x, y);
	}
}

void preDisplay(void)
{
	// move camera in view direction
	/*
	0   4   8   12  x
	1   5   9   13  y
	2   6   10  14  z
	*/
	if (keyDown['w']) {
		cameraPos[0] += modelView[2] * walkSpeed;
		cameraPos[1] += modelView[6] * walkSpeed;
		cameraPos[2] += modelView[10] * walkSpeed;
	}
	if (keyDown['s']) {
		cameraPos[0] -= modelView[2] * walkSpeed;
		cameraPos[1] -= modelView[6] * walkSpeed;
		cameraPos[2] -= modelView[10] * walkSpeed;
	}
	if (keyDown['a']) {
		cameraPos[0] += modelView[0] * walkSpeed;
		cameraPos[1] += modelView[4] * walkSpeed;
		cameraPos[2] += modelView[8] * walkSpeed;
	}
	if (keyDown['d']) {
		cameraPos[0] -= modelView[0] * walkSpeed;
		cameraPos[1] -= modelView[4] * walkSpeed;
		cameraPos[2] -= modelView[8] * walkSpeed;
	}
	if (keyDown['e']) {
		cameraPos[0] += modelView[1] * walkSpeed;
		cameraPos[1] += modelView[5] * walkSpeed;
		cameraPos[2] += modelView[9] * walkSpeed;
	}
	if (keyDown['q']) {
		cameraPos[0] -= modelView[1] * walkSpeed;
		cameraPos[1] -= modelView[5] * walkSpeed;
		cameraPos[2] -= modelView[9] * walkSpeed;
	}

	if (animateEmitter) {
		const float speed = 0.02;
		cursorPos.x = sin(currentTime*speed)*1.5;
		cursorPos.y = 1.5 + sin(currentTime*speed*1.3);
		cursorPos.z = cos(currentTime*speed)*1.5;
	}
	// move camera
	if (cameraPos[1] > 0.0f) cameraPos[1] = 0.0f;
	cameraPosLag += (cameraPos - cameraPosLag) * inertia;
	cameraRotLag += (cameraRot - cameraRotLag) * inertia;
	cursorPosLag += (cursorPos - cursorPosLag) * inertia;
	//glutPostRedisplay();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(cameraRotLag[0], 1.0, 0.0, 0.0);
	glRotatef(cameraRotLag[1], 0.0, 1.0, 0.0);
	glTranslatef(cameraPosLag[0], cameraPosLag[1], cameraPosLag[2]);

	glGetFloatv(GL_MODELVIEW_MATRIX, cameraModelView);
}

// initialize sliders
void initParams()
{
	// create a new parameter list
	params = new ParamListGL("misc");

	params->AddParam(new Param<int>("displayed slices", numDisplayedSlices, 0, 256, 1, &numDisplayedSlices));

	params->AddParam(new Param<float>("time step", timestep, 0.0, 0.02, 0.001, &timestep));

	params->AddParam(new Param<int>("timer span", timer_span, 0, 100, 1, &timer_span));
	params->AddParam(new Param<unsigned int>("batch num", initBatchN, 1, 10000, 100, &initBatchN));
	params->AddParam(new Param<unsigned int>("batch span", m_batch_span, 1, 100, 1, &m_batch_span));
	params->AddParam(new Param<int>("path num", showPathN, 1, psystem->getMaxPathN(), 1, &showPathN));

	SimParams &p = psystem->getParams();
	//params->AddParam(new Param<float>("damping", 0.99, 0.0, 1.0, 0.001, &p.globalDamping));
	//params->AddParam(new Param<float>("gravity", 0.0, 0.01, -0.01, 0.0001, &p.gravity.y));

	//params->AddParam(new Param<float>("noise freq", 0.1, 0.0, 1.0, 0.001, &p.noiseFreq));
	//params->AddParam(new Param<float>("noise strength", 0.001, 0.0, 0.01, 0.001, &p.noiseAmp));
	//params->AddParam(new Param<float>("noise anim", 0.0, -0.001, 0.001, 0.0001, &p.noiseSpeed.y));

	params->AddParam(new Param<float>("sprite size", spriteSize, 0.0, 0.5, 0.01, &spriteSize));
	params->AddParam(new Param<float>("alpha", alpha, 0.0, 1.0, 0.001, &alpha));

	//params->AddParam(new Param<float>("light color r", lightColor[0], 0.0, 1.0, 0.01, &lightColor[0]));
	//params->AddParam(new Param<float>("light color g", lightColor[1], 0.0, 1.0, 0.01, &lightColor[1]));
	//params->AddParam(new Param<float>("light color b", lightColor[2], 0.0, 1.0, 0.01, &lightColor[2]));

	//params->AddParam(new Param<float>("atten color r", colorAttenuation[0], 0.0, 1.0, 0.01, &colorAttenuation[0]));
	//params->AddParam(new Param<float>("atten color g", colorAttenuation[1], 0.0, 1.0, 0.01, &colorAttenuation[1]));
	//params->AddParam(new Param<float>("atten color b", colorAttenuation[2], 0.0, 1.0, 0.01, &colorAttenuation[2]));
	params->AddParam(new Param<float>("shadow alpha", shadowAlpha, 0.0, 0.1, 0.001, &shadowAlpha));

	//params->AddParam(new Param<float>("blur radius", blurRadius, 0.0, 10.0, 0.1, &blurRadius));
	params->AddParam(new Param<float>("source radius", sourceRadius, 0.0, 0.5, 0.002, &sourceRadius));
	params->AddParam(new Param<float>("eye separation", eyesep, 0.0, 1.0, 0.01, &eyesep));
	params->AddParam(new Param<float>("focal length", focallength, 0.0, 20.0, 0.5, &focallength));
	params->AddParam(new Param<float>("camera aperture", aperture, 0.0, 90.0, 1, &aperture));


	//params->AddParam(new Param<float>("emitter radius", emitterRadius, 0.0, 2.0, 0.01, &emitterRadius));
	//params->AddParam(new Param<uint>("emitter rate", emitterRate, 0, 10000, 1, &emitterRate));
	//params->AddParam(new Param<float>("emitter velocity", emitterVel, 0.0, 0.1, 0.001, &emitterVel));
	//params->AddParam(new Param<float>("emitter spread", emitterSpread, 0.0, 0.1, 0.001, &emitterSpread));

	params->AddParam(new Param<float>("particle lifetime", particleLifetime, 0.0, 20.0, 0.1, &particleLifetime));
}

void mainMenu(int i)
{
	key((unsigned char) i, 0, 0);
}

void initMenus()
{
	glutCreateMenu(mainMenu);
	glutAddMenuEntry("Reset block [1]", '1');
	glutAddMenuEntry("Toggle emitter [2]", '2');
	glutAddMenuEntry("Toggle animation [ ]", ' ');
	glutAddMenuEntry("Step animation [ret]", 13);
	glutAddMenuEntry("View mode [v]", 'v');
	glutAddMenuEntry("Move cursor mode [m]", 'm');
	glutAddMenuEntry("Move light mode [l]", 'l');
	glutAddMenuEntry("Toggle point rendering [p]", 'p');
	glutAddMenuEntry("Toggle sliders [h]", 'h');
	glutAddMenuEntry("Toggle sorting [o]", 'o');
	glutAddMenuEntry("Toggle vectors [V]", 'V');
	glutAddMenuEntry("Display light buffer [D]", 'D');
	glutAddMenuEntry("Toggle shadow blur [b]", 'b');
	glutAddMenuEntry("Increase no. slices [=]", '=');
	glutAddMenuEntry("Decrease no. slices [-]", '-');
	glutAddMenuEntry("Quit (esc)", '\033');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

GLuint createTexture(GLenum target, GLint internalformat, GLenum format, int w, int h, void *data)
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(target, tex);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(target, 0, internalformat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
	return tex;
}

GLuint loadTexture(char *filename)
{
	unsigned char *data = 0;
	unsigned int width, height;
	cutilCheckError( cutLoadPPM4ub(filename, &data, &width, &height));
	if (!data) {
		printf("Error opening file '%s'\n", filename);
		return 0;
	}
	printf("Loaded '%s', %d x %d pixels\n", filename, width, height);

	return createTexture(GL_TEXTURE_2D, GL_RGBA8, GL_RGBA, width, height, data);
}

// initialize OpenGL
void initGL(int argc, char **argv)
{
	//   glutInit(&argc, argv);
	//   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	//glutInitWindowSize(winWidth, winHeight);
	//glutCreateWindow("CUDA Smoke Particles");

	glewInit();
	if (!glewIsSupported("GL_VERSION_2_0 GL_VERSION_1_5 GL_ARB_multitexture GL_ARB_vertex_buffer_object")) {
		fprintf(stderr, "Required OpenGL extensions missing.");
		exit(-1);
	}

#if defined (_WIN32)
	if (wglewIsSupported("WGL_EXT_swap_control")) {
		// disable vertical sync
		wglSwapIntervalEXT(0);
	}
#endif

	glEnable(GL_DEPTH_TEST);

	// load floor texture
	char* imagePath = cutFindFilePath("floortile.ppm", argv[0]);

	//***********************************************************************************************
	char* sky_imagePath = cutFindFilePath("sky.ppm", argv[0]);
	char* skyfloor_imagePath = cutFindFilePath("skyfloortile.ppm", argv[0]);
	//***********************************************************************************************

	if (imagePath == 0) {
		fprintf(stderr, "Error finding floor image file\n");
		exit(EXIT_FAILURE);
	}
	floorTex = loadTexture(imagePath);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	//***********************************************************************************************
	skyTex=loadTexture(sky_imagePath);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	skyTexfloor=loadTexture(skyfloor_imagePath);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	//***********************************************************************************************

	floorProg = new GLSLProgram(floorVS, floorPS);	

	//***********************************************************************************************
	skyProg=new GLSLProgram(skyVS,skyPS);

	sky_obj=new skymesh();
	cgtk_obj=new cgtk();

	cgtk_obj->init();

	//bildProg=new GLSLProgram(bildVS,bildPS);
	//***********************************************************************************************

	glutReportErrors();
}


////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
void
initPaticleSystem(int argc, char** argv) 
{
	printf("[ %s ]\n", sSDKsample); 

	if (argc > 1) {
		cutGetCmdLineArgumenti( argc, (const char**) argv, "n", (int *) &numParticles);
		if (cutCheckCmdLineFlag(argc, (const char **)argv, "qatest") ||
			cutCheckCmdLineFlag(argc, (const char **)argv, "noprompt")
			) 
		{
			g_bQAReadback = true;
		}
		if (cutCheckCmdLineFlag(argc, (const char **)argv, "glverify")) 
		{
			g_bQAGLVerify = true;
		}
	}

	if (g_bQAReadback) {
		// For Automated testing, we do not use OpenGL/CUDA interop
		if ( cutCheckCmdLineFlag(argc, (const char **)argv, "device")) {
			cutilDeviceInit (argc, argv);
		} else {
			cudaSetDevice (cutGetMaxGflopsDeviceId() );
		}

		g_CheckRender = new CheckBackBuffer(winWidth, winHeight, 4, false);
		g_CheckRender->setPixelFormat(GL_RGBA);
		g_CheckRender->setExecPath(argv[0]);
		g_CheckRender->EnableQAReadback(true);

		// This code path is used for Automated Testing
		initParticles(numParticles, false, false);
		initParams();

		if (emitterOn) {
			runEmitter();
		}
		SimParams &params = psystem->getParams();
		params.cursorPos = make_float3(cursorPosLag.x, cursorPosLag.y, cursorPosLag.z);

		//psystem->step(timestep);

		float4 *pos = NULL, *vel = NULL;
		int g_TotalErrors = 0;

		psystem->dumpBin(&pos, &vel);

		g_CheckRender->dumpBin(pos, numParticles*sizeof(float4), "smokeParticles_pos.bin");
		g_CheckRender->dumpBin(vel, numParticles*sizeof(float4), "smokeParticles_vel.bin");

		if (!g_CheckRender->compareBin2BinFloat("smokeParticles_pos.bin", sRefBin[0], numParticles*sizeof(float4), MAX_EPSILON_ERROR, THRESHOLD))
			g_TotalErrors++;

		if (!g_CheckRender->compareBin2BinFloat("smokeParticles_vel.bin", sRefBin[1], numParticles*sizeof(float4), MAX_EPSILON_ERROR, THRESHOLD))
			g_TotalErrors++;


		delete psystem;
		delete g_CheckRender;

		printf("%s\n", (g_TotalErrors > 0) ? "FAILED" : "PASSED");

		cudaThreadExit();
	} else {
		// Normal smokeParticles rendering path
		// 1st initialize OpenGL context, so we can properly set the GL for CUDA.
		// This is needed to achieve optimal performance with OpenGL/CUDA interop.
		initGL( argc, argv );

		if ( cutCheckCmdLineFlag(argc, (const char **)argv, "device")) {
			cutilGLDeviceInit (argc, argv);
		} else {
			cudaGLSetGLDevice (cutGetMaxGflopsDeviceId() );
		}

		if (g_bQAGLVerify) {
			g_CheckRender = new CheckBackBuffer(winWidth, winHeight, 4);
			g_CheckRender->setPixelFormat(GL_RGBA);
			g_CheckRender->setExecPath(argv[0]);
			g_CheckRender->EnableQAReadback(true);
		}

		// This is the normal code path for SmokeParticles
		initParticles(numParticles, true, true);
		initParams();
		//        initMenus();

		//         glutDisplayFunc(display);
		//         glutReshapeFunc(reshape);
		//         glutMouseFunc(mouse);
		//         glutMotionFunc(motion);
		//         glutKeyboardFunc(key);
		//         glutKeyboardUpFunc(keyUp);
		//         glutSpecialFunc(special);
		//         glutIdleFunc(idle);
		// 
		//         glutMainLoop();
	}


}
void drawTargetCursor()
{

	glEnable(GL_DEPTH_TEST);
	glPushMatrix();

	vec3f v = vec3f(0.0, 0.0, 1.0);
	vec3f vd;
	ixform(v, vd, cameraModelView);
	vd=normalize(vd)*focallength;
	vd=-cameraPosLag-vd;
	glTranslatef(vd.x,vd.y,vd.z);
	glColor3f(0,0,1);
	glutWireSphere(0.05, 10, 10);
	glPopMatrix();
	//glDisable(GL_LIGHTING);
}
void exitPaticleSystem(int argc, char** argv) 
{
	cleanup();
	cutilExit(argc, argv);
}
