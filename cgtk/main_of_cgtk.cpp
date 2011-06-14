#include "cgtk.h"

#define MAX_KEYS 256

bool gKeys[MAX_KEYS];

cgtk *obj;

// for contiunes keypresses
void ProcessKeys()
{
	// Process keys
	for (int i = 0; i < 256; i++)
	{
		if (!gKeys[i])  { continue; }
		switch (i)
		{
		case ' ':
			break;
		case 'w':
			obj->stepsize += 1.0/2048.0;
			if(obj->stepsize > 0.25) obj->stepsize = 0.25;
			break;
		case 'e':
			obj->stepsize -= 1.0/2048.0;
			if(obj->stepsize <= 1.0/200.0) obj->stepsize = 1.0/200.0;
			break;
		}
	}

}

void key(unsigned char k, int x, int y)
{
	gKeys[k] = true;
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

	switch (key)
	{
	case 27 :
		{
			exit(0); break; 
		}
	case ' ':
		obj->toggle_visuals = !obj->toggle_visuals;
		break;
	}
}

// glut idle function
void idle_func()
{
	ProcessKeys();
	glutPostRedisplay();
}

void reshape_ortho(int w, int h)
{
	if (h == 0) h = 1;
	glViewport(0, 0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
}

void display()
{
	obj->display();
}

void resize(int w, int h)
{
	obj->resize(w,h);
}

int main(int argc, char * argv[])
{
	obj=new cgtk();

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("GPU raycasting tutorial");
	glutReshapeWindow(obj->WINDOW_SIZE,obj->WINDOW_SIZE);
	glutKeyboardFunc(key);
	glutKeyboardUpFunc(KeyboardUpCallback);
	
	glutDisplayFunc(display);
	glutIdleFunc(idle_func);
	glutReshapeFunc(resize);
	//obj->resize(obj->WINDOW_SIZE,obj->WINDOW_SIZE);
	obj->init();
	glutMainLoop();

	delete obj;
	return 0;
}