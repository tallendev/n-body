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

#include <thread>
#include <mutex>

#include "body.h"
#include "motion.h"
#include "utils.h"

int main();//(int argc, char* argv[]);
void simulate();
void glfw_init();
void initialize_gl();
void display(float*, float*, float*);
void render_setup();
void dump_bodies();

const int N = 16384;//1024;//2000;//98304;

const int SCREEN_W = 1920;
const int SCREEN_H = 1080;

//seconds, for now.
const float TS =  1;//.000000000001f;

static Body bodies[N];
static GLFWwindow* window;
static GLuint vbo;

GLchar *vertex_source;
GLchar *fragment_source;
GLuint vertexshader;
GLuint fragmentshader;
GLuint shaderprogram;

static float pos[N * 4];
static float colors[N * 4];


static const double sample_rate = .016;

std::mutex render_m;
std::mutex sim_m;

std::thread sim_t;

int main()//(int argc, char* argv[])
{
    srand(time(NULL));
    float x = 0;
    float y = 0;
    float z = 0;
    // for now set bodies as random...
    for (size_t i = 0; i < N; i++)
    {
        if (i == 0)
        {
            //bodies[i].set_mass(1000);//.1);
        }
        for (size_t j = 0; j < Body::DIMS; j++)
        {
            if (j == 0)
            {
                bodies[i].set_pos(j, x); 
            }
            else if (j == 1)
            {
                bodies[i].set_pos(j, y);
            }
            else
            {
                bodies[i].set_pos(j, z); 
            }
        }
        x = (((float) rand()) / (RAND_MAX / 2)) - 1;
        y = (((float) rand()) / (RAND_MAX / 2)) - 1;
    }

    std::cerr << &bodies[0] << " " << &bodies[0].acc << " " << &bodies[0].vel << " " << &bodies[0].pos << std::endl;
    
    glfw_init();
    initialize_gl();
    glewInit();
    render_setup();
    simulate();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


void simulate()
{
    int frames = 0;

    float* mass = new float[N];
    float* posx = new float[N];
    float* posy = new float[N];
    float* posz = new float[N];
    float* velx = new float[N];
    float* vely = new float[N];
    float* velz = new float[N];
    float* accx = new float[N];
    float* accy = new float[N];
    float* accz = new float[N];

    for (int i = 0; i < N; i++)
    {
        mass[i] = bodies[i].get_mass();
        posx[i] = bodies[i].get_pos(0);
        posy[i] = bodies[i].get_pos(1);
        posz[i] = bodies[i].get_pos(2);
        velx[i] = bodies[i].get_vel(0);
        vely[i] = bodies[i].get_vel(1);
        velz[i] = bodies[i].get_vel(2);
        accx[i] = bodies[i].get_acc(0);
        accy[i] = bodies[i].get_acc(1);
        accz[i] = bodies[i].get_acc(2);
    }

    float* gmass;
    float* gposx;
    float* gposy;
    float* gposz;
    float* gvelx;
    float* gvely;
    float* gvelz;
    float* gaccx;
    float* gaccy;
    float* gaccz;

    cudaStream_t stream1;
    cudaStream_t stream2;
    cudaStream_t stream3;
    cudaStream_t stream4;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);
    cudaStreamCreate(&stream3);
    cudaStreamCreate(&stream4);
//    #ifdef cudaerr
//        gpuErrchk(cudaMalloc((void**)&gmass, sizeof(mass)));
//        gpuErrchk(cudaMemcpyAsync(gmass, mass, sizeof(mass), cudaMemcpyHostToDevice, stream1));
//    #else
        cudaMalloc((void**)&gmass, N * sizeof(float));
        cudaMemcpyAsync(gmass, mass, N * sizeof(float), cudaMemcpyHostToDevice, stream1);
        cudaMalloc((void**)&gposx, N * sizeof(float));
        cudaMemcpyAsync(gposx, posx,N * sizeof(float) , cudaMemcpyHostToDevice, stream1);

        cudaMalloc((void**)&gposy, N * sizeof(float));
        cudaMemcpyAsync(gposy, posy, N * sizeof(float), cudaMemcpyHostToDevice, stream1);

        cudaMalloc((void**)&gposz, N * sizeof(float));
        cudaMemcpyAsync(gposz, posz, N * sizeof(float), cudaMemcpyHostToDevice, stream1);

        cudaMalloc((void**)&gvelx, N * sizeof(float));
        cudaMemcpyAsync(gvelx, velx, N * sizeof(float), cudaMemcpyHostToDevice, stream1);
        cudaMalloc((void**)&gvely, N * sizeof(float));
        cudaMemcpyAsync(gvely, vely, N * sizeof(float), cudaMemcpyHostToDevice, stream1);
        cudaMalloc((void**)&gvelz, N * sizeof(float));
        cudaMemcpyAsync(gvelz, velz, N * sizeof(float), cudaMemcpyHostToDevice, stream1);
        cudaMalloc((void**)&gaccx, N * sizeof(float));
        cudaMemcpyAsync(gaccx, accx, N * sizeof(float), cudaMemcpyHostToDevice, stream1);
        cudaMalloc((void**)&gaccy, N * sizeof(float));
        cudaMemcpyAsync(gaccy, accy, N * sizeof(float), cudaMemcpyHostToDevice, stream1);
        cudaMalloc((void**)&gaccz, N * sizeof(float));
        cudaMemcpyAsync(gaccz, accz, N * sizeof(float), cudaMemcpyHostToDevice, stream1);
//    #endif
    double time = 0;
    while (!glfwWindowShouldClose(window) && (time = glfwGetTime()) < 60)
    {
        sim_t = std::thread([stream2, stream3, gmass, gposx, gposy, 
                              gposz, gvelx, gvely, gvelz, gaccx, gaccy, gaccz]() 
                            {run_calculations(N, TS, stream2, stream3, 
                                               gmass, gposx, gposy, gposz,
                                               gvelx, gvely, gvelz, gaccx,
                                               gaccy, gaccz);});
        //display(posx, posy, posz);
        frames++;
//        #ifdef cudaerr
//        #else
            cudaMemcpyAsync(posx, gposx, N * sizeof(float), 
                            cudaMemcpyDeviceToHost, stream4);
            cudaMemcpyAsync(posy, gposy, N * sizeof(float), 
                            cudaMemcpyDeviceToHost, stream4);
            cudaMemcpyAsync(posz, gposz, N * sizeof(float),
                            cudaMemcpyDeviceToHost, stream4);
//        #endif
        sim_t.join();
    }
    cudaFree(mass);
    cudaFree(gposx);
    cudaFree(gposy);
    cudaFree(gposz);
    cudaFree(gvelx);
    cudaFree(gvely);
    cudaFree(gvelz);
    cudaFree(gaccx);
    cudaFree(gaccy);
    cudaFree(gaccz);
    delete[] mass;
    delete[] posx;
    delete[] posy;
    delete[] posz;
    delete[] velx;
    delete[] vely;
    delete[] velz;
    delete[] accx;
    delete[] accy;
    delete[] accz;
    std::cerr << "Total Frames: " << frames << std::endl;
    std::cerr << "Done!" << std::endl;
}

void render_setup()
{
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(pos) + sizeof(colors), NULL, GL_STATIC_DRAW);
    
    shaderprogram = InitShader("shader.vert", "shader.frag");
    glUseProgram(shaderprogram);

    GLuint vPosition = glGetAttribLocation(shaderprogram, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(shaderprogram, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(pos)));

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
}

void  glfw_init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW" << std::endl;
        exit(1);
    }
    window = glfwCreateWindow(SCREEN_W, SCREEN_H, "n-body test", glfwGetPrimaryMonitor(), NULL);
 //   window = glfwCreateWindow(SCREEN_W, SCREEN_H, "n-body test", NULL, NULL);
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
    glViewport(0, 0, SCREEN_W, SCREEN_H);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void display(float* posx, float* posy, float* posz)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < N; i++)
    {
        int idx = i * 4;
        pos[idx] = posx[i];
        pos[idx + 1] = posy[i];
        pos[idx + 2] = posz[i];
        pos[idx + 3] = 1.0;
        colors[idx] = bodies[i].get_color(0);
        colors[idx + 1] = bodies[i].get_color(1);
        colors[idx + 2] = bodies[i].get_color(2);
        colors[idx + 3] = 1.0;
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pos), pos);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pos), sizeof(colors), colors);

    glPointSize(.0001);
    glDrawArrays(GL_POINTS, 0, N);

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
