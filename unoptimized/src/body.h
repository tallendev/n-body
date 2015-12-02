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
        Body(double mass, double radius);
        __device__ __host__ void set_mass(double mass);
        __device__ __host__ void set_radius(double radius);
        __device__ __host__ double get_mass();
        __device__ __host__ double get_radius();
        __device__ __host__ double get_color(size_t idx);
        __device__ __host__ double get_pos(size_t idx);
        __device__ __host__ double get_vel(size_t idx);
        __device__ __host__ double get_acc(size_t idx);
        __device__ __host__ void set_pos(size_t idx, double val);
        __device__ __host__ void set_vel(size_t idx, double val);
        __device__ __host__ void set_acc(size_t idx, double val);
        __device__ __host__ void set_color(size_t idx, double val);
        __device__ __host__ void update_pos(size_t idx, double delta);
        __device__ __host__ void update_vel(size_t idx, double delta);
        __device__ __host__ void update_acc(size_t idx, double delta);
        //void render();
        std::string to_string();
    private:
        double mass;
        double radius;
        double acc[DIMS];
        double vel[DIMS];
        double pos[DIMS];
        double color[DIMS];
        GLUquadricObj* body;
};


#endif
