#ifndef MOTION_H
#define MOTION_H
void run_calculations(int N, Body* g_bodies, double timestep);
extern "C" __global__ void calculate_force(int, Body*, double timestep);
extern "C" __global__ void update_pos(int, Body*);
#endif
