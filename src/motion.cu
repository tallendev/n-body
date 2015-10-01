#include <GL/glew.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <device_launch_parameters.h>

#include <iostream>

#include "body.h"
#include "motion.h"
#include "utils.h"

// gravitational constant
const double G = 6.674E-11;

const int BLOCK_THREADS = 2048;

void run_calculations(int N, Body* g_bodies, double timestep)
{
    int blocks = N / BLOCK_THREADS;   
    calculate_force<<<blocks, BLOCK_THREADS>>>(N, g_bodies, timestep);
    gpuErrchk(cudaPeekAtLastError());
    gpuErrchk(cudaDeviceSynchronize());
    update_pos<<<blocks, BLOCK_THREADS>>>(N, g_bodies);
    gpuErrchk(cudaPeekAtLastError());
    gpuErrchk(cudaDeviceSynchronize());
}

extern "C" __global__
void calculate_force(int nbodies, Body* g_bodies, double timestep)
{
    double dx;
    double dy;
    double dz;
    double dist;
    double mass;
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    if (i < nbodies)
    {
        for (int j = 0; j < nbodies; j++)
        {
            if (i != j)
            {
                dx = g_bodies[j].get_pos(0) - g_bodies[i].get_pos(0);
                dy = g_bodies[j].get_pos(1) - g_bodies[i].get_pos(1);
                dz = g_bodies[j].get_pos(2) - g_bodies[i].get_pos(2);
                dist = sqrt(dx * dx + dy * dy + dz * dz);
                dist = dist * dist * dist;
                //omit mass of current body, must divide by it later to get accel anyway
                mass = timestep * G * g_bodies[j].get_mass();
                g_bodies[i].update_acc(0, mass * dx / dist);
                g_bodies[i].update_acc(1, mass * dx / dist);
                g_bodies[i].update_acc(2, mass * dx / dist);
            }
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
    g_bodies[i].update_acc(0, g_bodies[i].get_vel(0));
    g_bodies[i].update_acc(1, g_bodies[i].get_vel(1));
    g_bodies[i].update_acc(2, g_bodies[i].get_vel(2));
}
