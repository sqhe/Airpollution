#pragma once

#ifndef _CGTKH_
#define _CGTKH_


#include <windows.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cassert>
#include "Vector3.h"
#include "dat.h"

using namespace std;

class cgtk
{
public:
	cgtk();
	~cgtk();
	void cgErrorCallback();
	void create_volumetexture();
	void load_volumetexture(char *file);
	void load_volumetexture_j(char *file);
	void load_volumetexture_k(char *file);
	void load_volumetexture_step(char *file);
	void load_vertex_program(CGprogram &v_program,char *shader_path, char *program_name);
	void load_fragment_program(CGprogram &f_program,char *shader_path, char *program_name);
	void text_generate(re_dat_tet *obj,float *pminmax,bool whi);
	void set_tex_param(char* par, GLuint tex,const CGprogram &program,CGparameter param);
	void enable_renderbuffers();
	void disable_renderbuffers();
	//void vertex(float x, float y, float z);
	void vertex(float x, float y, float z,float *b);
	//void drawQuads(float x, float y, float z);
	void drawQuads(float x, float y, float z,float *b);
	void resize(int w,int h);
	void reshape_ortho(int w, int h);
	void draw_fullscreen_quad();
	void render_buffer_to_screen();
	void render_backface();
	void raycasting_pass();
	void display();
	void init();
	void reset_fbo();
	void setb(float *a);
	void drawBd();
public:
	int WINDOW_SIZE;
	int WINDOW_SIZE_W;
	int WINDOW_SIZE_H;
	int WINDOW_SIZE_D;
	int VOLUME_TEX_SIZE;
	float *b;
	bool toggle_visuals;
	CGcontext context; 
	CGprofile vertexProfile, fragmentProfile; 
	CGparameter param1,param2;
	GLuint renderbuffer; 
	GLuint framebuffer; 
	CGprogram vertex_main,fragment_main; // the raycasting shader programs
	GLuint volume_texture; // the volume texture
	GLuint backface_buffer; // the FBO buffers
	//GLuint backface_buffer0; // the copy of FBO buffers
	GLuint final_image;
	float stepsize ;



};

#endif