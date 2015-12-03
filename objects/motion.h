#ifndef MOTION_H
#define MOTION_H
void run_calculations(int N, Body* g_bodies, float timestep, cudaStream_t stream1, cudaStream_t stream2);
extern "C" __global__ void calculate_force(int, Body*, float timestep);
extern "C" __global__ void update_pos(int, Body*);
#endif
