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

const int BLOCK_THREADS = 1024;//512;
#if __CUDA_ARCH__ >= 200
    const int maxThreadsPerBlock = 1024;
    const int minBlocks = 2;
#else
    const int maxThreadsPerBlock = 512 / 2;
    const int minBlocks = 2;
#endif

void run_calculations(int N, float timestep, 
                        cudaStream_t stream1, cudaStream_t stream2, float* mass,
                        float* posx, float* posy, float* posz, float* velx, 
                        float* vely, float* velz, float* accx, float* accy,
                        float* accz)
{
    int blocks = N / BLOCK_THREADS;
    blocks = N % BLOCK_THREADS == 0 ? blocks : blocks + 1;

    calculate_force<<<blocks, BLOCK_THREADS, sizeof(float) * 4, stream1>>>(N,  timestep,
                mass, posx, posy, posz, accx, accy, accz);
    #ifdef cudaerr
        gpuErrchk(cudaPeekAtLastError());
        gpuErrchk(cudaDeviceSynchronize());
    #endif
    update_pos<<<blocks, BLOCK_THREADS, 0, stream2>>>(N, posx, posy,
                posz, velx, vely, velz, accx, accy, accz);
    #ifdef cudaerr
        gpuErrchk(cudaPeekAtLastError());
        gpuErrchk(cudaDeviceSynchronize());
    #endif
}

extern "C" __global__
//__launch_bounds__(maxThreadsPerBlock, minBlocks)
void calculate_force(int nbodies, float timestep, float* gmass,
                     float* posx, float* posy, float* posz, float* accx, float*
                     accy, float* accz)
{
    float dx;
    float dy;
    float dz;
    float dist;
    float mass;
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    #pragma unroll 16
    for (int j = 0; j < nbodies; j++)
    {
        //__shared__ float x, y, z, mass;
        //x = posx[j];
        //y = posy[j];
        //z = posz[j];
    //    mass = timestep * G;
//        __syncthreads();
        if (j != i)
        {
            //dx = x - posx[i];
            //dy = y - posy[i];
            //dz = z - posz[i];
            dx = posx[j] - posx[i];
            dy = posy[j] - posy[i];
            dz = posz[j] - posz[i];

            //dist = sqrt(dx * dx + dy * dy + dz * dz);
            //dist = dist * dist * dist;
            dist = (dx * dx) + ((dy * dy) + (dz * dz));
            dist = (1.0f/dist) * sqrtf(dist) + epsilon;
            //dist = dist < 1.0f ? dist : 1.0f;
            //omit mass of current body, must divide by it later to get accel anyway
            mass = timestep * G * gmass[j];
      //      mass = mass * gmass[j];

            accx[i] += mass * dx * dist;
            accy[i] += mass * dy * dist;
            accz[i] += mass * dz * dist;
        }
    }
}

extern "C" __global__
//__launch_bounds__(maxThreadsPerBlock, minBlocks)
void update_pos(int nbodies, 
                     float* posx, float* posy, float* posz, float* velx, float*
                     vely, float* velz, float* accx, float* accy, float* accz)
{
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    velx[i] += accx[i];
    vely[i] += accy[i];
    velz[i] += accz[i];
    posx[i] += velx[i];
    posy[i] += vely[i];
    posz[i] += velz[i];
}
