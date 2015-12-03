#ifndef MOTION_H
#define MOTION_H
void run_calculations(int N, float timestep, 
                       cudaStream_t stream1, cudaStream_t stream2, float*, 
                       float*, float*, float*, float*, float*, float*, float*, 
                       float*, float*);
extern "C" __global__ void calculate_force(int, float timestep, float*, float*, float*, float*, float*, float*, float*);
extern "C" __global__ void update_pos(int, float*, float*, float*, float*, float*, float*, float*, float*, float*);

#endif
