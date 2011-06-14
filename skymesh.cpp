#include "StdAfx.h"
#include "skymesh.h"

skymesh::skymesh()
{
	//AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

	//memset(TextureImage,0,sizeof(void *)*1);           	// Set The Pointer To NULL

	//TextureImage[0]=auxDIBImageLoad("data\\sky.bmp");

	//glGenTextures(1, &skyTex);

	//glBindTexture(GL_TEXTURE_2D, skyTex);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);


	std::string s;
	char ss;


	std::ifstream ifile("data\\Sky2.obj");

	while(ifile>>s)
	{
		if(s=="v")
		{
			std::vector<float> tem;

			float ftem[3];

			ifile>>ftem[0]>>ftem[1]>>ftem[2];

			for(int i=0;i<3;i++)
				tem.push_back(ftem[i]);

			point.push_back(tem);
		}

		if(s=="vt")
		{
			int ss=0;

			std::vector<float> tem;

			float ftem[2];

			ifile>>ftem[0]>>ftem[1];

			for(int i=0;i<2;i++)
				tem.push_back(ftem[i]);

			tex.push_back(tem);

		}

		if(s=="f")
		{
			std::vector<int> tem1,tem2;

			int ptm[3],ttm[3];

			ifile>>ptm[0]>>ss>>ttm[0]>>ptm[1]>>ss>>ttm[1]>>ptm[2]>>ss>>ttm[2];

			for(int i=0;i<3;i++)
			{
				tem1.push_back(ptm[i]);
				tem2.push_back(ttm[i]);
			}

			fpoint.push_back(tem1);
			ftex.push_back(tem2);
		}
	}
		

	ifile.close();}

skymesh::~skymesh()
{
	//if(TextureImage[0])
	//{
	//	if(TextureImage[0]->data)
	//		free(TextureImage[0]->data);

	//	free(TextureImage[0]);
	//}

}

void	skymesh::draw(int uu)
{
	//glBindTexture(GL_TEXTURE_2D, skyTex);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

	//glDisable(GL_LIGHTING);

	//glEnable(GL_TEXTURE_2D);

	glColor3f(1.0, 1.0, 1.0);
	glNormal3f(0.0, 1.0, 0.0);

    glBegin(GL_TRIANGLES);
    {

		for(int i=0;i<ftex.size();i++)
		{
			glTexCoord2f(tex[(ftex[i][0]-1)][0],tex[(ftex[i][0]-1)][1]); 
			glVertex3f((point[(fpoint[i][0]-1)][0])*uu,(point[(fpoint[i][0]-1)][2]-3.9018)*uu,(point[(fpoint[i][0]-1)][1])*uu);	// 1
			glTexCoord2f(tex[(ftex[i][1]-1)][0],tex[(ftex[i][1]-1)][1]);
			glVertex3f((point[(fpoint[i][1]-1)][0])*uu,(point[(fpoint[i][1]-1)][2]-3.9018)*uu,(point[(fpoint[i][1]-1)][1])*uu);	// 2
			glTexCoord2f(tex[(ftex[i][2]-1)][0],tex[(ftex[i][2]-1)][1]);
			glVertex3f((point[(fpoint[i][2]-1)][0])*uu,(point[(fpoint[i][2]-1)][2]-3.9018)*uu,(point[(fpoint[i][2]-1)][1])*uu);	// 3
			
		}

    }
    glEnd();

	glDisable(GL_TEXTURE_2D);

	//glEnable(GL_LIGHTING);
}