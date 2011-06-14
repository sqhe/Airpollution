#ifndef __SMOKESYSTEM_H__
#define __SMOKESYSTEM_H__
#include "dat.h"
#define min_1(a,b)            (((a) < (b)) ? (a) : (b))
extern int winWidth;
extern int winHeight;
extern unsigned int numParticles;
extern int timer_span;
extern bool bStereo;
extern float activeArea[6];//bounding box, xmin,xmax,ymin,ymax,zmin,zmax
enum M_MODE{ M_VIEW = 0, M_MOVE_CURSOR, M_MOVE_LIGHT, M_MOVE_SOURCE, M_MOVE_ACTIVEAREA,M_SCALE_ACTIVEAREA };
extern int mode;
enum ParticleConfig
{
	CONFIG_RANDOM,
	CONFIG_GRID,
	CONFIG_AIR_RANDOM,
	CONFIG_INLET_RANDOM,
	CONFIG_SPHERE_RANDOM,
	_NUM_CONFIGS
};
extern ParticleConfig beingInit;
extern unsigned int initCount;
extern unsigned int initBatchN;
extern unsigned int m_cur_step;
//mode
extern bool displayEnabled;
extern bool displaySliders;
extern bool displaySkybox;
extern bool displayTemprature;

void initPaticleSystem(int argc, char** argv);
void exitPaticleSystem(int argc, char** argv);
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void movelight(bool s);
void DrawBuilding();
void DrawBounding();
void DrawAirData();
void setAirData();
void key(unsigned char key, int /*x*/, int /*y*/);
void keyUp(unsigned char key, int /*x*/, int /*y*/);
void updatePaticles();
bool resetPaticles(ParticleConfig config,unsigned int beginIndex,unsigned int num);
void drawPathes();
void setCgWindow(int w,int h);
void setLight();
void set3dtexture();
void setsunpowertransfer();
//int		PickObject(int mx, int my);
bool initparticlesInShpere(unsigned int beginIndex,unsigned int num);
bool initparticlesInRandomVolumn(unsigned int beginIndex,unsigned int num);
bool initparticlesInRandomPlain(unsigned int beginIndex,unsigned int num);
void drawSphereSource();
void drawAciveArea();
void preDisplay(void);
void setLeftView();
void setRightView();
void drawTargetCursor();
extern re_dat_tet *obj;
#endif // __SMOKESYSTEM_H__
