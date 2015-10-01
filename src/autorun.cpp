#include <GL/glew.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <GLFW/glfw3.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <device_launch_parameters.h>

#include <helper_cuda.h>    // includes cuda.h and cuda_runtime_api.h
#include <helper_functions.h>
#include <helper_cuda_gl.h>

#include "body.h"
#include "motion.h"
#include "utils.h"

int main();//(int argc, char* argv[]);
void simulate();
void glfw_init();
void initialize_gl();
void display();
void dump_bodies();

const int N = 100;

const int SCREEN_W = 1920;
const int SCREEN_H = 1080;

//seconds, for now.
const double TS = .0001;

static Body bodies[N];
static GLFWwindow* window;

static double sample_rate = .016;

int main()//(int argc, char* argv[])
{
    srand(time(NULL));

    double x = SCREEN_W/2;
    double y = SCREEN_H/2;
    double z = 0;
    // for now set bodies as random...
    for (size_t i = 0; i < N; i++)
    {
        if (i == 0)
        {
            bodies[i].set_mass(.1);
        }
        for (size_t j = 0; j < Body::DIMS; j++)
        {
            if (j == 0)
                bodies[i].set_pos(j, x); 
            else if (j == 1)
                bodies[i].set_pos(j, y);
            else
                bodies[i].set_pos(j, z); 
        }
        x = rand() % (SCREEN_W / 2);
        y = rand() % (SCREEN_H / 2);   
    }
    
    dump_bodies();

    glfw_init();
    initialize_gl();
    glewInit();

    simulate();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


void simulate()
{
    double draw_timer = 0;
    Body* g_bodies;
    gpuErrchk(cudaMalloc((void**)&g_bodies, sizeof(bodies)));
    while (!glfwWindowShouldClose(window))
    {
        //std::cerr << sizeof(bodies) << std::endl;
        gpuErrchk(cudaMemcpy(g_bodies, bodies, sizeof(bodies), cudaMemcpyHostToDevice));
        run_calculations(N, g_bodies, TS);
        gpuErrchk(cudaMemcpy(bodies, g_bodies, sizeof(bodies), cudaMemcpyDeviceToHost));
        if (draw_timer > sample_rate)
        {
            draw_timer = 0;   
            display();
            std::cerr << "display()" << std::endl;   
            //dump_bodies();
        }
        draw_timer += TS;
    }
    cudaFree(g_bodies);
}

void  glfw_init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW" << std::endl;
        exit(1);
    }
    window = glfwCreateWindow(SCREEN_W, SCREEN_H, "n-body test", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create window." << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);
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
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < N; i++)
    {
        glPushMatrix();
        bodies[i].render();
        glPopMatrix();
    }
    glfwSwapBuffers(window);
    glFlush();
}


void dump_bodies()
{
    for (int i = 0; i < N; i++)
    {
        std::cerr << "Body " << i << ": " << bodies[i].to_string() << std::endl;
    }
}

