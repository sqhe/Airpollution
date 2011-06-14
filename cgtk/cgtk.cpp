#include "StdAfx.h"
#include "cgtk.h"

cgtk::cgtk()
{
	toggle_visuals=true;
	stepsize=1.0/100.0;
	WINDOW_SIZE=1024;
	WINDOW_SIZE_W=1024;
	WINDOW_SIZE_H=1024;
	WINDOW_SIZE_D=1024;
	VOLUME_TEX_SIZE=128;
	//VOLUME_TEX_SIZE=256;
	b=new float[6];

	for(int i=0;i<6;i+=2)
	{
		b[i]=0;
		b[i+1]=1;
	}

	b[0]=0;
	b[1]=1;
	b[2]=0;
	b[3]=1;
	b[4]=0;
	b[5]=1;
}

cgtk::~cgtk()
{
	if(volume_texture)
		glDeleteRenderbuffersEXT(1, &volume_texture);

	if(renderbuffer)
		glDeleteRenderbuffersEXT(1, &renderbuffer);

	if(final_image)
		glDeleteTexturesEXT(1, &final_image);

	if(backface_buffer)
		glDeleteTexturesEXT(1, &backface_buffer);

	if(framebuffer)
		glDeleteFramebuffersEXT(1, &framebuffer);

	if(b)
		delete [] b;
}

void cgtk::cgErrorCallback()
{
	CGerror lastError=cgGetError();
	if(lastError)
	{
		cout<< cgGetErrorString(lastError) <<endl;
		if(context!=NULL)
			cout<<cgGetLastListing(context) <<endl;
		exit(0);
	}
}

void cgtk::create_volumetexture()
{
	int size = VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE* 4;
	GLubyte *data = new GLubyte[size];

	for(int x = 0; x < VOLUME_TEX_SIZE; x++)
	{for(int y = 0; y < VOLUME_TEX_SIZE; y++)
	{for(int z = 0; z < VOLUME_TEX_SIZE; z++)
	{
		data[(x*4)   + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = z%250;
		data[(x*4)+1 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = y%250;
		data[(x*4)+2 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
		data[(x*4)+3 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 230;

		Vector3 p =	Vector3(x,y,z)- Vector3(VOLUME_TEX_SIZE-20,VOLUME_TEX_SIZE-30,VOLUME_TEX_SIZE-30);
		bool test = (p.length() < 42);
		if(test)
			data[(x*4)+3 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 0;

		p =	Vector3(x,y,z)- Vector3(VOLUME_TEX_SIZE/2,VOLUME_TEX_SIZE/2,VOLUME_TEX_SIZE/2);
		test = (p.length() < 24);
		if(test)
			data[(x*4)+3 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 0;


		if(x > 20 && x < 40 && y > 0 && y < VOLUME_TEX_SIZE && z > 10 &&  z < 50)
		{

			data[(x*4)   + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 100;
			data[(x*4)+1 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
			data[(x*4)+2 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = y%100;
			data[(x*4)+3 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
		}

		if(x > 50 && x < 70 && y > 0 && y < VOLUME_TEX_SIZE && z > 10 &&  z < 50)
		{

			data[(x*4)   + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
			data[(x*4)+1 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
			data[(x*4)+2 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = y%100;
			data[(x*4)+3 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
		}

		if(x > 80 && x < 100 && y > 0 && y < VOLUME_TEX_SIZE && z > 10 &&  z < 50)
		{

			data[(x*4)   + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
			data[(x*4)+1 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 70;
			data[(x*4)+2 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = y%100;
			data[(x*4)+3 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
		}

		p =	Vector3(x,y,z)- Vector3(24,24,24);
		test = (p.length() < 40);
		if(test)
			data[(x*4)+3 + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 0;


	}}}

	for(int i=0;i<VOLUME_TEX_SIZE;i++)
		for(int j=0;j<VOLUME_TEX_SIZE;j++)
			for(int k=0;k<VOLUME_TEX_SIZE;k++)
				data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=0;



	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volume_texture);
	glBindTexture(GL_TEXTURE_3D, volume_texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, VOLUME_TEX_SIZE, VOLUME_TEX_SIZE,VOLUME_TEX_SIZE,0, GL_RGBA, GL_UNSIGNED_BYTE,data);

	delete []data;
	cout << "volume texture created" << endl;
}

void cgtk::load_volumetexture(char *file)
{
	if(volume_texture)
		glDeleteRenderbuffersEXT(1, &volume_texture);

	int alpha_valua=250;
	int alpha_null=0;

	int size = VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE* 4;

	int *tex_data;

	tex_data=new int[size/4];

	ifstream ifile(file,ios::binary);

	ifile.read((char *)tex_data,(size/4)*sizeof(int));

	ifile.close();

	GLubyte *data = new GLubyte[size];

	for(int i=0;i<VOLUME_TEX_SIZE;i++)
	{
		for(int j=0;j<VOLUME_TEX_SIZE;j++)
		{
			for(int k=0;k<VOLUME_TEX_SIZE;k++)
			{
				int tep_tep,tep_alp;

				tep_tep=tex_data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE+j*VOLUME_TEX_SIZE+k];

				if(tep_tep==INT_MAX)
				{
					tep_tep=0;
					tep_alp=alpha_null;
				}
				else
				{
					tep_alp=alpha_valua;
				}

				data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=tep_tep;
				data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=tep_tep;
				data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=250-tep_tep;
				data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=tep_alp;

				if(tep_alp==alpha_null)
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=0;
			}
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volume_texture);
	glBindTexture(GL_TEXTURE_3D, volume_texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, VOLUME_TEX_SIZE, VOLUME_TEX_SIZE,VOLUME_TEX_SIZE,0, GL_RGBA, GL_UNSIGNED_BYTE,data);

	delete []data;
	delete [] tex_data;
	cout << "volume texture created" << endl;
}


void cgtk::load_volumetexture_j(char *file)
{
	if(volume_texture)
		glDeleteRenderbuffersEXT(1, &volume_texture);

	int alpha_valua=120;
	int alpha_null=10;

	int size = VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE* 4;

	int *tex_data;

	tex_data=new int[size/4];

	ifstream ifile(file,ios::binary);

	ifile.read((char *)tex_data,(size/4)*sizeof(int));

	ifile.close();

	GLubyte *data = new GLubyte[size];

	for(int i=0;i<VOLUME_TEX_SIZE;i++)
	{
		for(int j=0;j<VOLUME_TEX_SIZE;j++)
		{
			for(int k=0;k<VOLUME_TEX_SIZE;k++)
			{
				float m=fabs((float)(i-125));

				if(fabs((float)(j-125))<m)
					m=fabs((float)(j-125));

				if(fabs((float)(k-125))<m)
					m=fabs((float)(k-125));

				int tum;

				tum=(int)((1-m/125)*200);

				int tep_tep,tep_alp;

				tep_tep=tex_data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE+j*VOLUME_TEX_SIZE+k];

				if(tep_tep==INT_MAX)
				{
					tep_tep=0;
					tep_alp=alpha_null;
				}
				else
				{
					tep_alp=alpha_valua;
				}

				data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=(GLubyte)tep_tep;
				data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=(GLubyte)tep_tep;
				data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=(GLubyte)(250-tep_tep);
				data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=(GLubyte)tum;

				if(tep_alp==alpha_null)
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=80;
			}
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volume_texture);
	glBindTexture(GL_TEXTURE_3D, volume_texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, VOLUME_TEX_SIZE, VOLUME_TEX_SIZE,VOLUME_TEX_SIZE,0, GL_RGBA, GL_UNSIGNED_BYTE,data);

	delete []data;
	delete [] tex_data;
	cout << "volume texture created" << endl;
}

void cgtk::load_volumetexture_k(char *file)
{
	if(volume_texture)
		glDeleteRenderbuffersEXT(1, &volume_texture);

	int alpha_valua=75;
	int alpha_null=0;

	int size = VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE* 4;

	int *tex_data;

	tex_data=new int[size/4];

	ifstream ifile(file,ios::binary);

	ifile.read((char *)tex_data,(size/4)*sizeof(int));

	ifile.close();

	GLubyte *data = new GLubyte[size];

	for(int i=0;i<VOLUME_TEX_SIZE;i++)
	{
		for(int j=0;j<VOLUME_TEX_SIZE;j++)
		{
			for(int k=0;k<VOLUME_TEX_SIZE;k++)
			{
				//float m=fabs((float)(i-125));

				//if(fabs((float)(j-125))<m)
				//	m=fabs((float)(j-125));

				//if(fabs((float)(k-125))<m)
				//	m=fabs((float)(k-125));

				//int tum;

				//tum=(int)((1-m/125)*200);

				int tep_tep/*,tep_alp*/;

				tep_tep=tex_data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE+j*VOLUME_TEX_SIZE+k];

				double tplg;

				if(tep_tep!=(-1))
				{
					tplg=(log((double)(1+tep_tep)))/log(256.0);

					tep_tep=(int)(tplg*255);

					tplg=(log((double)(1+tep_tep)))/log(256.0);

					tep_tep=(int)(tplg*255);
				}

				//else
				//{
				//	tep_tep=-1;
				//}

				//if(tep_tep==-1)
				//{
				//	tep_tep=-1;
				//	tep_alp=alpha_null;
				//}
				//else
				//{
				//	tep_alp=alpha_valua;
				//}

				if(tep_tep!=(-1))
				{
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=(GLubyte)tep_tep;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=(GLubyte)tep_tep/*(255-tep_tep)*/;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=(GLubyte)(255-tep_tep);
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=(GLubyte)alpha_valua;
				}
				else
				{
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=(GLubyte)180;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=(GLubyte)180;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=(GLubyte)0;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=(GLubyte)alpha_null;
				}

				//if(tep_alp==alpha_null)
				//{
				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=50;

				//	data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=(GLubyte)((i*178+80)%250);
				//	data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=(GLubyte)((j*157+100)%250);
				//	data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=(GLubyte)((k*169+54)%250);
				//	data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=(GLubyte)((k*i*j*178+87)%250);;

				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=(GLubyte)200;
				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=(GLubyte)200;
				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=(GLubyte)50;
				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=(GLubyte)50;
				//}
			}
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volume_texture);
	glBindTexture(GL_TEXTURE_3D, volume_texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, VOLUME_TEX_SIZE, VOLUME_TEX_SIZE,VOLUME_TEX_SIZE,0, GL_RGBA, GL_UNSIGNED_BYTE,data);

	delete []data;
	delete [] tex_data;
	cout << "volume texture created" << endl;
}

void cgtk::load_volumetexture_step(char *file)
{
	if(volume_texture)
		glDeleteRenderbuffersEXT(1, &volume_texture);

	int alpha_valua=150;
	int alpha_null=90;

	int size = VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE* 4;

	int *tex_data;

	tex_data=new int[size];

	ifstream ifile(file,ios::binary);

	ifile.read((char *)tex_data,size*sizeof(int));

	ifile.close();

	GLubyte *data = new GLubyte[size];

	for(int i=0;i<size;i++)
		data[i]=(GLubyte)(tex_data[i]);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volume_texture);
	glBindTexture(GL_TEXTURE_3D, volume_texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, VOLUME_TEX_SIZE, VOLUME_TEX_SIZE,VOLUME_TEX_SIZE,0, GL_RGBA, GL_UNSIGNED_BYTE,data);

	delete []data;
	delete [] tex_data;
	cout << "volume texture created" << endl;


}
void cgtk::load_vertex_program(CGprogram &v_program,char *shader_path, char *program_name)
{
	assert(cgIsContext(context));
	v_program = cgCreateProgramFromFile(context, CG_SOURCE,shader_path,
		vertexProfile,program_name, NULL);	
	if (!cgIsProgramCompiled(v_program))
		cgCompileProgram(v_program);

	cgGLEnableProfile(vertexProfile);
	cgGLLoadProgram(v_program);
	cgGLDisableProfile(vertexProfile);

}

void cgtk::load_fragment_program(CGprogram &f_program,char *shader_path, char *program_name)
{
	assert(cgIsContext(context));
	f_program = cgCreateProgramFromFile(context, CG_SOURCE, shader_path,
		fragmentProfile,program_name, NULL);	
	if (!cgIsProgramCompiled(f_program))
		cgCompileProgram(f_program);

	cgGLEnableProfile(fragmentProfile);
	cgGLLoadProgram(f_program);
	cgGLDisableProfile(fragmentProfile);
}

void cgtk::set_tex_param(char* par, GLuint tex,const CGprogram &program,CGparameter param)
{
	param = cgGetNamedParameter(program, par); 
	cgGLSetTextureParameter(param, tex); 
	cgGLEnableTextureParameter(param);
}

void cgtk::enable_renderbuffers()
{
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, framebuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
}

void cgtk::disable_renderbuffers()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

//void cgtk::vertex(float x, float y, float z)
//{
//	glColor3f(x,y,z);
//	glMultiTexCoord3fARB(GL_TEXTURE1_ARB, WINDOW_SIZE*x, WINDOW_SIZE*y, WINDOW_SIZE*z);
//	glVertex3f(x,y,z);
//}

void cgtk::vertex(float x, float y, float z,float *b)
{
	if(((b[1]-b[0])==0)||((b[3]-b[2])==0)||((b[5]-b[4])==0))
	{
		cout<<"bound equals 0!"<<endl;
		exit(0);
	}

	glColor3f((x-b[0])/(b[1]-b[0]),(y-b[2])/(b[3]-b[2]),(z-b[4])/(b[5]-b[4]));
	glMultiTexCoord3fARB(GL_TEXTURE1_ARB, (WINDOW_SIZE*(x-b[0])/(b[1]-b[0])), (WINDOW_SIZE*(y-b[2])/(b[3]-b[2])), (WINDOW_SIZE*(z-b[4])/(b[5]-b[4])));
	glVertex3f(x,y,z);
}

//void cgtk::drawQuads(float x, float y, float z)
//{
//	glBegin(GL_QUADS);
//	/* Back side */
//	glNormal3f(0.0, 0.0, -1.0);
//	vertex(0.0, 0.0, 0.0);
//	vertex(0.0, y, 0.0);
//	vertex(x, y, 0.0);
//	vertex(x, 0.0, 0.0);
//
//	/* Front side */
//	glNormal3f(0.0, 0.0, 1.0);
//	vertex(0.0, 0.0, z);
//	vertex(x, 0.0, z);
//	vertex(x, y, z);
//	vertex(0.0, y, z);
//
//	/* Top side */
//	glNormal3f(0.0, 1.0, 0.0);
//	vertex(0.0, y, 0.0);
//	vertex(0.0, y, z);
//    vertex(x, y, z);
//	vertex(x, y, 0.0);
//
//	/* Bottom side */
//	glNormal3f(0.0, -1.0, 0.0);
//	vertex(0.0, 0.0, 0.0);
//	vertex(x, 0.0, 0.0);
//	vertex(x, 0.0, z);
//	vertex(0.0, 0.0, z);
//
//	/* Left side */
//	glNormal3f(-1.0, 0.0, 0.0);
//	vertex(0.0, 0.0, 0.0);
//	vertex(0.0, 0.0, z);
//	vertex(0.0, y, z);
//	vertex(0.0, y, 0.0);
//
//	/* Right side */
//	glNormal3f(1.0, 0.0, 0.0);
//	vertex(x, 0.0, 0.0);
//	vertex(x, y, 0.0);
//	vertex(x, y, z);
//	vertex(x, 0.0, z);
//	glEnd();
//}

void cgtk::drawQuads(float x, float y, float z,float *b)
{
	glBegin(GL_QUADS);
	/* Back side */
	glNormal3f(0.0, 0.0, -1.0);
	vertex(0.0, 0.0, 0.0,b);
	vertex(0.0, y, 0.0,b);
	vertex(x, y, 0.0,b);
	vertex(x, 0.0, 0.0,b);

	/* Front side */
	glNormal3f(0.0, 0.0, 1.0);
	vertex(0.0, 0.0, z,b);
	vertex(x, 0.0, z,b);
	vertex(x, y, z,b);
	vertex(0.0, y, z,b);

	/* Top side */
	glNormal3f(0.0, 1.0, 0.0);
	vertex(0.0, y, 0.0,b);
	vertex(0.0, y, z,b);
	vertex(x, y, z,b);
	vertex(x, y, 0.0,b);

	/* Bottom side */
	glNormal3f(0.0, -1.0, 0.0);
	vertex(0.0, 0.0, 0.0,b);
	vertex(x, 0.0, 0.0,b);
	vertex(x, 0.0, z,b);
	vertex(0.0, 0.0, z,b);

	/* Left side */
	glNormal3f(-1.0, 0.0, 0.0);
	vertex(0.0, 0.0, 0.0,b);
	vertex(0.0, 0.0, z,b);
	vertex(0.0, y, z,b);
	vertex(0.0, y, 0.0,b);

	/* Right side */
	glNormal3f(1.0, 0.0, 0.0);
	vertex(x, 0.0, 0.0,b);
	vertex(x, y, 0.0,b);
	vertex(x, y, z,b);
	vertex(x, 0.0, z,b);
	glEnd();
}

void cgtk::resize(int w,int h)
{
	//if (h == 0) h = 1;
	////glViewport(0, 0, w, h);
	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	//glLoadIdentity();
	//gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 0.01, 400.0);
	//glLoadIdentity();
	//glPopMatrix();
	//glMatrixMode(GL_MODELVIEW);
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w/(GLfloat)h, 0.01, 400.0);
	glMatrixMode(GL_MODELVIEW);
}

void cgtk::reshape_ortho(int w, int h)
{
	//if (h == 0) h = 1;
	//glViewport(0, 0,w,h);
	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	//glLoadIdentity();
	//gluOrtho2D(0, 1, 0, 1);
	////glPushMatrix();
	//glLoadIdentity();
	//glPopMatrix();
	////glPopMatrix();
	//glMatrixMode(GL_MODELVIEW);
	if (h == 0) h = 1;
	//glViewport(0, 0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glPopMatrix();
}

void cgtk::draw_fullscreen_quad()
{
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);

	glTexCoord2f(0,0); 
	glVertex2f(0,0);

	glTexCoord2f(WINDOW_SIZE,0); 
	glVertex2f(1,0);

	glTexCoord2f(WINDOW_SIZE, WINDOW_SIZE); 

	glVertex2f(1, 1);
	glTexCoord2f(0, WINDOW_SIZE); 
	glVertex2f(0, 1);

	glEnd();
	glEnable(GL_DEPTH_TEST);
}

void cgtk::render_buffer_to_screen()
{
	//glEnable(GL_BLEND);

	////glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadIdentity();
	//glEnable(GL_TEXTURE_RECTANGLE_ARB);
	//if(toggle_visuals)
	//	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,final_image);
	//else
	//	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,backface_buffer);
	//reshape_ortho(WINDOW_SIZE,WINDOW_SIZE);
	//draw_fullscreen_quad();
	//glDisable(GL_TEXTURE_RECTANGLE_ARB);
	//glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();

	glEnable(GL_BLEND);

	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	glLoadIdentity();
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	if(toggle_visuals)
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB,final_image);
	else
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB,backface_buffer);
	reshape_ortho(WINDOW_SIZE_W,WINDOW_SIZE_H);
	draw_fullscreen_quad();
	glDisable(GL_TEXTURE_RECTANGLE_ARB);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glPopMatrix();



}

void cgtk::render_backface()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_RECTANGLE_ARB,backface_buffer,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	//drawQuads(1,1,1,b);
	drawBd();
	glDisable(GL_CULL_FACE);
}

void cgtk::raycasting_pass()
{
	disable_renderbuffers();

	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_RECTANGLE_ARB,final_image,0);
	//glClearColor(0,0,0,0);
	//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	cgGLEnableProfile(vertexProfile);
	cgGLEnableProfile(fragmentProfile);
	cgGLBindProgram(vertex_main);
	cgGLBindProgram(fragment_main);
	cgGLSetParameter1f(cgGetNamedParameter(fragment_main,"stepsize"),stepsize);
	cgGLSetParameter1f(cgGetNamedParameter(fragment_main,"width"),WINDOW_SIZE_W);
	cgGLSetParameter1f(cgGetNamedParameter(fragment_main,"heigth"),WINDOW_SIZE_H);
	set_tex_param("tex",backface_buffer,fragment_main,param1);
	set_tex_param("volume_tex",volume_texture,fragment_main,param2);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//drawQuads(1,1,1,b);
	drawBd();
	cgGLDisableProfile(vertexProfile);
	cgGLDisableProfile(fragmentProfile);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void cgtk::display()
{
	//static float rotate=0;
	//rotate += 0.25;

	//resize(WINDOW_SIZE,WINDOW_SIZE);

	//enable_renderbuffers();

	////glMatrixMode(GL_MODELVIEW);
	////glPushMatrix();
	//glLoadIdentity();
	//glTranslatef(0,0,-2.25);
	//glRotatef(rotate,0,1,1);
	//glTranslatef(-0.5,-0.5,-0.5);


	//render_backface();
	//raycasting_pass();
	//disable_renderbuffers();
	//render_buffer_to_screen();
	////glutSwapBuffers();

	//float *b;

	//b=new float[6];

	//for(int i=0;i<6;i+=2)
	//{
	//	b[i]=0;
	//	b[i+1]=2;
	//}


	static float rotate=0;
	rotate += 0.25;

	//resize(WINDOW_SIZE_W,WINDOW_SIZE_H);

	enable_renderbuffers();

	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadIdentity();
	//glTranslatef(0,0,-2.25);
	//glRotatef(rotate,0,1,1);

	//glTranslatef(-0.5,-0.5,-0.5);
	render_backface();
	raycasting_pass();
	//disable_renderbuffers();
	//render_buffer_to_screen();
	//glutSwapBuffers();
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glPopMatrix();

	//delete [] b;
}

void cgtk::init()
{
	cout << "glew init " << endl;
	GLenum err = glewInit();

	// initialize all the OpenGL extensions
	glewGetExtension("glMultiTexCoord2fvARB");  
	if(glewGetExtension("GL_EXT_framebuffer_object") )cout << "GL_EXT_framebuffer_object support " << endl;
	if(glewGetExtension("GL_EXT_renderbuffer_object"))cout << "GL_EXT_renderbuffer_object support " << endl;
	if(glewGetExtension("GL_ARB_vertex_buffer_object")) cout << "GL_ARB_vertex_buffer_object support" << endl;
	if(GL_ARB_multitexture)cout << "GL_ARB_multitexture support " << endl;

	if (glewGetExtension("GL_ARB_fragment_shader")      != GL_TRUE ||
		glewGetExtension("GL_ARB_vertex_shader")        != GL_TRUE ||
		glewGetExtension("GL_ARB_shader_objects")       != GL_TRUE ||
		glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
	{
		cout << "Driver does not support OpenGL Shading Language" << endl;
		exit(1);
	}

	//glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0);
	create_volumetexture();
	//load_volumetexture_k("data\\output31_int.bin");
	//load_volumetexture_step("data\\step_by_step2.bin");

	// CG init
	//cgSetErrorCallback(cgErrorCallback);
	context = cgCreateContext();
	if (cgGLIsProfileSupported(CG_PROFILE_VP40))
	{
		vertexProfile = CG_PROFILE_VP40;
		cout << "CG_PROFILE_VP40 supported." << endl; 
	}
	else 
	{
		if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
			vertexProfile = CG_PROFILE_ARBVP1;
		else
		{
			cout << "Neither arbvp1 or vp40 vertex profiles supported on this system." << endl;
			exit(1);
		}
	}

	if (cgGLIsProfileSupported(CG_PROFILE_FP40))
	{
		fragmentProfile = CG_PROFILE_FP40;
		cout << "CG_PROFILE_FP40 supported." << endl; 
	}
	else 
	{
		if (cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
			fragmentProfile = CG_PROFILE_ARBFP1;
		else
		{
			cout << "Neither arbfp1 or fp40 fragment profiles supported on this system." << endl;
			exit(1);
		}
	}

	// load the vertex and fragment raycasting programs
	load_vertex_program(vertex_main,"raycasting_shader.cg","vertex_main");
	cgErrorCallback();
	load_fragment_program(fragment_main,"raycasting_shader.cg","fragment_main");
	cgErrorCallback();

	// Create the to FBO's one for the backside of the volumecube and one for the finalimage rendering
	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,framebuffer);

	glGenTextures(1, &backface_buffer);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, backface_buffer);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,GL_RGBA16F_ARB, WINDOW_SIZE_W, WINDOW_SIZE_H, 0, GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, backface_buffer, 0);

	glGenTextures(1, &final_image);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, final_image);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,GL_RGBA16F_ARB, WINDOW_SIZE_W, WINDOW_SIZE_H, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenRenderbuffersEXT(1, &renderbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, WINDOW_SIZE_W, WINDOW_SIZE_H);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void cgtk::reset_fbo()
{
	if(renderbuffer)
		glDeleteRenderbuffersEXT(1, &renderbuffer);

	if(final_image)
		glDeleteTexturesEXT(1, &final_image);

	if(backface_buffer)
		glDeleteTexturesEXT(1, &backface_buffer);

	if(framebuffer)
		glDeleteFramebuffersEXT(1, &framebuffer);

	// Create the to FBO's one for the backside of the volumecube and one for the finalimage rendering
	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,framebuffer);

	glGenTextures(1, &backface_buffer);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, backface_buffer);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,GL_RGBA16F_ARB, WINDOW_SIZE_W, WINDOW_SIZE_H, 0, GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, backface_buffer, 0);

	glGenTextures(1, &final_image);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, final_image);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,GL_RGBA16F_ARB, WINDOW_SIZE_W, WINDOW_SIZE_H, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenRenderbuffersEXT(1, &renderbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, WINDOW_SIZE_W, WINDOW_SIZE_H);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void cgtk::setb(float *a)
{
	for(int i=0;i<6;i++)
		b[i]=a[i];
}

void cgtk::drawBd()
{
	glBegin(GL_QUADS);
	/* Back side */
	glNormal3f(0.0, 0.0, -1.0);
	vertex(b[0], b[2], b[4],b);
	vertex(b[0], b[3], b[4],b);
	vertex(b[1], b[3], b[4],b);
	vertex(b[1], b[2], b[4],b);

	/* Front side */
	glNormal3f(0.0, 0.0, 1.0);
	vertex(b[0], b[2], b[5],b);
	vertex(b[1], b[2], b[5],b);
	vertex(b[1], b[3], b[5],b);
	vertex(b[0], b[3], b[5],b);

	/* Top side */
	glNormal3f(0.0, 1.0, 0.0);
	vertex(b[0], b[3], b[4],b);
	vertex(b[0], b[3], b[5],b);
	vertex(b[1], b[3], b[5],b);
	vertex(b[1], b[3], b[4],b);

	/* Bottom side */
	glNormal3f(0, -1.0, 0.0);
	vertex(b[0], b[2], b[4],b);
	vertex(b[1], b[2], b[4],b);
	vertex(b[1], b[2], b[5],b);
	vertex(b[0], b[2], b[5],b);

	/* Left side */
	glNormal3f(-1.0, 0.0, 0.0);
	vertex(b[0], b[2], b[4],b);
	vertex(b[0], b[2], b[5],b);
	vertex(b[0], b[3], b[5],b);
	vertex(b[0], b[3], b[4],b);

	/* Right side */
	glNormal3f(1.0, 0.0, 0.0);
	vertex(b[1], b[2], b[4],b);
	vertex(b[1], b[3], b[4],b);
	vertex(b[1], b[3], b[5],b);
	vertex(b[1], b[2], b[5],b);
	glEnd();
}

void cgtk::text_generate(re_dat_tet *obj,float *pminmax,bool whi)
{
	int size_xyz[3];

	for(int i=0;i<3;i++)
		size_xyz[i]=VOLUME_TEX_SIZE;

	obj->tex_3d(size_xyz,pminmax,whi);

	if(volume_texture)
		glDeleteRenderbuffersEXT(1, &volume_texture);

	int alpha_valua=75;
	int alpha_null=0;

	int size = VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE* 4;


	GLubyte *data = new GLubyte[size];

	for(int i=0;i<VOLUME_TEX_SIZE;i++)
	{
		for(int j=0;j<VOLUME_TEX_SIZE;j++)
		{
			for(int k=0;k<VOLUME_TEX_SIZE;k++)
			{
				//float m=fabs((float)(i-125));

				//if(fabs((float)(j-125))<m)
				//	m=fabs((float)(j-125));

				//if(fabs((float)(k-125))<m)
				//	m=fabs((float)(k-125));

				//int tum;

				//tum=(int)((1-m/125)*200);

				int tep_tep/*,tep_alp*/;

				tep_tep=obj->p_tex_int[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE+j*VOLUME_TEX_SIZE+k];

				double tplg;

				if(tep_tep!=(-1))
				{
					tplg=(log((double)(1+tep_tep)))/log(256.0);

					tep_tep=(int)(tplg*255);

					tplg=(log((double)(1+tep_tep)))/log(256.0);

					tep_tep=(int)(tplg*255);
				}

				//else
				//{
				//	tep_tep=-1;
				//}

				//if(tep_tep==-1)
				//{
				//	tep_tep=-1;
				//	tep_alp=alpha_null;
				//}
				//else
				//{
				//	tep_alp=alpha_valua;
				//}

				if(tep_tep!=(-1))
				{
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=(GLubyte)tep_tep;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=(GLubyte)tep_tep/*(255-tep_tep)*/;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=(GLubyte)(255-tep_tep);
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=(GLubyte)alpha_valua;
				}
				else
				{
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=(GLubyte)180;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=(GLubyte)180;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=(GLubyte)0;
					data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=(GLubyte)alpha_null;
				}

				//if(tep_alp==alpha_null)
				//{
				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=50;

				//	data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=(GLubyte)((i*178+80)%250);
				//	data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=(GLubyte)((j*157+100)%250);
				//	data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=(GLubyte)((k*169+54)%250);
				//	data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=(GLubyte)((k*i*j*178+87)%250);;

				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+0]=(GLubyte)200;
				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+1]=(GLubyte)200;
				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+2]=(GLubyte)50;
				//	//data[i*VOLUME_TEX_SIZE*VOLUME_TEX_SIZE*4+j*VOLUME_TEX_SIZE*4+k*4+3]=(GLubyte)50;
				//}
			}
		}
	}
//***************************************************************************************
/*
	int *tep;
	tep=new int[size];
	for (int i=0;i<size;i++)
	{
		tep[i]=(int)(data[i]);
	}

	ofstream ofil("lintex.b",ios::binary);

	ofil.write((char *)tep,size*sizeof(int));

	ofil.close();

	delete [] tep;
*/
//***************************************************************************************


	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volume_texture);
	glBindTexture(GL_TEXTURE_3D, volume_texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, VOLUME_TEX_SIZE, VOLUME_TEX_SIZE,VOLUME_TEX_SIZE,0, GL_RGBA, GL_UNSIGNED_BYTE,data);

	delete []data;
	delete [] obj->p_tex_int;
	obj->p_tex_int=NULL;
	cout << "volume texture created" << endl;
}