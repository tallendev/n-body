#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>


#include <GL/glew.h>
#include <GL/glut.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <vector_types.h>
#include <vector_functions.h>

#include <helper_cuda.h>    // includes cuda.h and cuda_runtime_api.h
#include <helper_functions.h>
#include <helper_cuda_gl.h>

#include "body.h"


int main(int argc, char* argv[]);
void glut_init(int*, char**);
void initialize_gl();
void display();
void test(int);
void dump_bodies();

const int N = 1000;
const int THREADS = N;

const int SCREEN_W = 1600;
const int SCREEN_H = 900;

Body bodies[N];

int main(int argc, char* argv[])
{
    srand(time(NULL));

    double x = 800;
    double y = 450;
    double z = 0;
    // for now set bodies as a grid...
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < Body::DIMS; j++)
        {
            if (j == 0)
                bodies[i].set_pos(j, x); 
            else if (j == 1)
                bodies[i].set_pos(j, y);
            else
                bodies[i].set_pos(j, z); 
        }
        x = rand() % SCREEN_W;
        y = rand() % SCREEN_H;   
    }
    
    dump_bodies();

    glut_init(&argc, argv);
    initialize_gl();
    glewInit();
    
    glutTimerFunc(100, test, 0);
    glutMainLoop();
    return 0;
}

void  glut_init(int* argc, char* argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(SCREEN_W, SCREEN_H);
    glutCreateWindow("n-body test");
    glutDisplayFunc(display);
}

void initialize_gl()
{
    std::cerr << "GL: ";
    std::cerr << glGetString(GL_VERSION);
    std::cerr << " ";
    std::cerr << glGetString(GL_VENDOR);
    std::cerr << std::endl;

    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    glViewport(0, 0, SCREEN_W, SCREEN_H);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_W, 0, SCREEN_H, 0, 1000.0);
}

void display()
{
	static GLUquadricObj	*body = gluNewQuadric();
	int			slices=16, stacks=16;
	double			radius=10.0;//5.0;

    gluQuadricDrawStyle(body,GLU_FILL);
    gluQuadricNormals(body,GLU_FLAT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < N; i++)
    {
//        glColor3f(1.0, 1.0, 1.0);//glColor3f(color[0], color[1], color[2]);
  //      glTranslated(bodies[i].get_pos(0), bodies[i].get_pos(1), 0); //bodies[i].get_pos(2));
        //gluDisk(body, 0.0, radius, slices, stacks); 
    //    gluSphere(body, radius, slices, stacks); 
        glPushMatrix();
        bodies[i].render();
        glPopMatrix();
    }
    glutSwapBuffers();
    glFlush();
}

void test(int lineno)
{
    glutPostRedisplay();
    glutTimerFunc(100, test, 0);
}

void dump_bodies()
{
    for (int i = 0; i < N; i++)
    {
        std::cerr << "Body " << i << ": " << bodies[i].to_string() << std::endl;
    }
}
