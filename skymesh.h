#pragma once

#ifndef _SKYMESH_
#define _SKYMESH_

#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glaux.h>
#include <fstream>
#include <string>


class skymesh
{
	public:
					skymesh();
					~skymesh();
		void		draw(int uu);

	private:

		//GLuint									skyTex;
		std::vector<std::vector<float>>			point;
		std::vector<std::vector<float>>			tex;
		std::vector<std::vector<int>>			fpoint;
		std::vector<std::vector<int>>			ftex;
		//AUX_RGBImageRec							 *TextureImage[1];
};

#endif