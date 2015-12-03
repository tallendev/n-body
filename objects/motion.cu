#include <GL/glew.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <device_launch_parameters.h>

#include <iostream>

#include "body.h"
#include "motion.h"
#include "utils.h"

// gravitational constant
const float G = 6.674E-11;
const float epsilon = .25f * .25f;

const int BLOCK_THREADS = 256;//1024;

void run_calculations(int N, Body* g_bodies, float timestep, cudaStream_t stream1, cudaStream_t stream2)
{
    int blocks = N / BLOCK_THREADS;
    blocks = N % BLOCK_THREADS == 0 ? blocks : blocks + 1;

    calculate_force<<<blocks, BLOCK_THREADS, 0, stream1>>>(N, g_bodies, timestep);
    #ifdef cudaerr
        gpuErrchk(cudaPeekAtLastError());
        gpuErrchk(cudaDeviceSynchronize());
    #endif
    update_pos<<<blocks, BLOCK_THREADS, 0, stream2>>>(N, g_bodies);
    #ifdef cudaerr
        gpuErrchk(cudaPeekAtLastError());
        gpuErrchk(cudaDeviceSynchronize());
    #endif
}

extern "C" __global__
void calculate_force(int nbodies, Body* g_bodies, float timestep)
{
    float dx;
    float dy;
    float dz;
    float dist;
    float mass;
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    for (int j = 0; j < nbodies; j++)
    {
        if (j != i)
        {
            dx = g_bodies[j].get_pos(0) - g_bodies[i].get_pos(0);
            dy = g_bodies[j].get_pos(1) - g_bodies[i].get_pos(1);
            dz = g_bodies[j].get_pos(2) - g_bodies[i].get_pos(2);


            //dist = sqrt(dx * dx + dy * dy + dz * dz);
            //dist = dist * dist * dist;
            dist = dx * dx + dy * dy + dz * dz;
            dist = (1.0f/dist) * sqrtf(dist) + epsilon;
            //dist = dist < 1.0f ? dist : 1.0f;
            //omit mass of current body, must divide by it later to get accel anyway
            mass = timestep * G * g_bodies[j].get_mass();

            //g_bodies[i].update_acc(0, mass * dx / dist);
            //g_bodies[i].update_acc(1, mass * dy / dist);
            //g_bodies[i].update_acc(2, mass * dz / dist);
            g_bodies[i].update_acc(0, mass * dx * dist);
            g_bodies[i].update_acc(1, mass * dy * dist);
            g_bodies[i].update_acc(2, mass * dz * dist);
        }
    }
}

extern "C" __global__
void update_pos(int nbodies, Body* g_bodies)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    g_bodies[i].update_vel(0, g_bodies[i].get_acc(0));
    g_bodies[i].update_vel(1, g_bodies[i].get_acc(1));
    g_bodies[i].update_vel(2, g_bodies[i].get_acc(2));
    g_bodies[i].update_pos(0, g_bodies[i].get_vel(0));
    g_bodies[i].update_pos(1, g_bodies[i].get_vel(1));
    g_bodies[i].update_pos(2, g_bodies[i].get_vel(2));
}
