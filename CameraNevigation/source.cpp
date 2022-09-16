#include <iostream>
#include "GL\glut.h"
#include "Mesh.h"

float zoom = 15.0f;
float rotx = 0;
float roty = 0.001f;
float tx = 0;
float ty = 0;
int lastx=0;
int lasty=0;
unsigned char Buttons[3] = {0};

Mesh *m_Mesh;
int DrawMode = 0;
int SmoothingType = 0;

void Init(void) 
{
	glEnable(GL_DEPTH_TEST);
	int iter = 50;
	
	switch (SmoothingType)
	{
	case 0:
		m_Mesh->Smoothing(iter);
		printf("[Selected method] Simple smoothing\n");
		break;
	case 1:
		m_Mesh->LaplacianSmoothing(iter);
		printf("[Selected method] Laplacian smoothing\n");
		break;
	case 2:
		m_Mesh->TaubinSmoothing(iter);
		printf("[Selected method] Taubin smoothing\n");
		break;
	case 3:
		m_Mesh->LaplacianSmoothingWithCotan(iter);
		printf("[Selected method] Laplacian smoothing with cotangent weight\n");
		break;
	}
}

void Rendering(void)
{
	// Init lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	m_Mesh->Draw(DrawMode);

	glDisable(GL_LIGHTING);

}

void Display(void)
{
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//printf("%f, %f, %f\n", tx, ty, zoom);
	//tx = 0.9;
	//ty = 0.6;
	zoom = 7.0;

	glTranslatef(0,0,-zoom);
	glTranslatef(tx,ty,0);
	glRotatef(rotx,1,0,0);
	glRotatef(roty,0,1,0);	
	
	Rendering();

	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	if(w==0) {
		h = 1;
	}
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,(float)w/h,0.1,100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Motion(int x,int y)
{
	int diffx=x-lastx;
	int diffy=y-lasty;
	lastx=x;
	lasty=y;

	if(Buttons[2]) {
		zoom -= (float) 0.05f * diffx;
	} else if(Buttons[0]) {
		rotx += (float) 0.5f * diffy;
		roty += (float) 0.5f * diffx;		
	} else if(Buttons[1]) {
		tx += (float) 0.05f * diffx;
		ty -= (float) 0.05f * diffy;
	}
	glutPostRedisplay();
}

void Mouse(int button,int state,int x,int y)
{
	lastx=x;
	lasty=y;
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		Buttons[0] = ((GLUT_DOWN==state)?1:0);
		break;
	case GLUT_MIDDLE_BUTTON:
		Buttons[1] = ((GLUT_DOWN==state)?1:0);
		break;
	case GLUT_RIGHT_BUTTON:
		Buttons[2] = ((GLUT_DOWN==state)?1:0);
		break;
	default:
		break;		
	}
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'Q':
	case 'q':
		exit(0);
	case 'W':
	case 'w':
		DrawMode = 1;
		break;
	case 'S':
	case 's':
		DrawMode = 0;
		break;
	case 'A':
	case 'a':
		m_Mesh->TaubinSmoothing();
		break;
	}
	glutPostRedisplay();
}

int main(int argc,char** argv)
{
	m_Mesh = new Mesh("OBJ\\dragon.obj");
	printf("\n0: Avg. smoothing\n1: Laplacian smoothing\n2: Taubin smoothing\n3: Cotant based Smoothing\n");
	//scanf("%d", &SmoothingType);
	SmoothingType = 3;
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(800,800);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Assignment #1 : Mesh Smoothing");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	Init();
	glutMainLoop();
}

























