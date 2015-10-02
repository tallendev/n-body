#include <GL/glew.h>
#include <GL/glext.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

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
void render_setup();
void dump_bodies();

const int N = 2;//100;

const int SCREEN_W = 1920;
const int SCREEN_H = 1080;

//seconds, for now.
const double TS = .0001;

static Body bodies[N];
static GLFWwindow* window;
static GLuint vbo;

GLchar *vertex_source;
GLchar *fragment_source;
GLuint vertexshader;
GLuint fragmentshader;
GLuint shaderprogram;

static double pos[N * 3];
static double colors[N * 3];

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
        //x = rand() % SCREEN_W;
        //y = rand() % SCREEN_H;
        x = ((double) rand()) / RAND_MAX;
        y = ((double) rand()) / RAND_MAX;
    }
    
    dump_bodies();

    glfw_init();
    initialize_gl();
    glewInit();
    render_setup();

    //simulate();
    while(true) display();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


void simulate()
{
    double draw_timer = 0;
    Body* g_bodies;
    gpuErrchk(cudaMalloc((void**)&g_bodies, sizeof(bodies)));
    gpuErrchk(cudaMemcpy(g_bodies, bodies, sizeof(bodies), cudaMemcpyHostToDevice));
    while (!glfwWindowShouldClose(window))
    {
        //std::cerr << sizeof(bodies) << std::endl;
        run_calculations(N, g_bodies, TS);
        if (draw_timer > sample_rate)
        {
            gpuErrchk(cudaMemcpy(bodies, g_bodies, sizeof(bodies), cudaMemcpyDeviceToHost));
            draw_timer = 0;   
            display();
            std::cerr << "display()" << std::endl;   
            dump_bodies();
        }
        draw_timer += TS;
    }
    cudaFree(g_bodies);
}

void render_setup()
{
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(pos) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pos), pos);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pos), sizeof(colors), colors);
    
    shaderprogram = InitShader("shader.vert", "shader.frag");
    glUseProgram(shaderprogram);

    GLuint vPosition = glGetAttribLocation(shaderprogram, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(shaderprogram, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pos)));
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
    glEnable(GL_POINT_SMOOTH);
    glViewport(0, 0, SCREEN_W, SCREEN_H);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_W, 0, SCREEN_H, 0, 1000.0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < N; i++)
    {
        int idx = i * 3;
        pos[idx] = bodies[i].get_pos(0);
        pos[idx + 1] = bodies[i].get_pos(1);
        pos[idx + 2] = bodies[i].get_pos(2);
        colors[idx] = bodies[i].get_color(0);
        colors[idx + 1] = bodies[i].get_color(1);
        colors[idx + 2] = bodies[i].get_color(2);
        /*
        glPushMatrix();
        bodies[i].render();
        glPopMatrix();
        */
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(GLfloat), pos);
    glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(GLfloat), 3 * sizeof(GLfloat), colors);

    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, N);
    //glDrawArrays(GL_TRIANGLES, 0, N);
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
