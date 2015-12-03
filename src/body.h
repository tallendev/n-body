#ifndef BODY_H
#define BODY_H

#include <GL/glew.h>

#include <array>
#include <cstddef>

class Body
{
    friend int main();
    public:
        static const size_t X = 0;
        static const size_t Y = 1;
        static const size_t Z = 2;
        static const size_t DIMS = 3;

        Body();
        Body(float mass, float radius);
        __device__ __host__ float get_mass();
        __device__ __host__ float get_radius();
        __device__ __host__ float get_color(size_t idx);
        __device__ __host__ float get_pos(size_t idx);
        __device__ __host__ float get_vel(size_t idx);
        __device__ __host__ float get_acc(size_t idx);
        __device__ __host__ void set_mass(float mass);
        __device__ __host__ void set_radius(float radius);
        __device__ __host__ void set_pos(size_t idx, float val);
        __device__ __host__ void set_vel(size_t idx, float val);
        __device__ __host__ void set_acc(size_t idx, float val);
        __device__ __host__ void set_color(size_t idx, float val);
        __device__ __host__ void update_pos(size_t idx, float delta);
        __device__ __host__ void update_vel(size_t idx, float delta);
        __device__ __host__ void update_acc(size_t idx, float delta);
        //void render();
        std::string to_string();
    private:
        float mass;
        float radius;
        float acc[DIMS];
        float vel[DIMS];
        float pos[DIMS];
        float color[DIMS];
};


#endif
