#include "body.h"

#include <sstream>
#include <string>
#include <iostream>

Body::Body() : Body::Body(.01, .75) {}

Body::Body(double mass, double radius)
{
    this->mass = mass;
    this->radius = radius;
    body = gluNewQuadric();

    gluQuadricDrawStyle(body, GLU_FILL);
    gluQuadricNormals(body, GLU_SMOOTH);
    gluQuadricOrientation(body, GLU_OUTSIDE);
    gluQuadricTexture(body, GL_TRUE);

    for (size_t i = 0; i < DIMS; i++)
    {
        set_pos(i, 0);                 
        set_vel(i, 0);                 
        set_acc(i, 0);                 
        set_color(i, 1.0);
    }
//    set_pos(3, 1);
//    set_color(3, 1);
}

extern "C" __device__ __host__
void Body::set_mass(double mass)
{
    this->mass = mass;
}

extern "C" __device__ __host__
void Body::set_radius(double radius)
{
    this->radius = radius;
}

extern "C" __device__ __host__
double Body::get_color(size_t idx)
{
    return color[idx];
}

extern "C" __device__ __host__
double Body::get_mass()
{
    return mass;
}

extern "C" __device__ __host__
double Body::get_radius()
{
    return radius;
}

extern "C" __device__ __host__
double Body::get_pos(size_t idx)
{
    return pos[idx];
}

extern "C" __device__ __host__
double Body::get_vel(size_t idx)
{
    return vel[idx];
}

extern "C" __device__ __host__
double Body::get_acc(size_t idx)
{
    return acc[idx];
}

extern "C" __device__ __host__
void Body::set_pos(size_t idx, double val)
{
    pos[idx] = val;
}

extern "C" __device__ __host__
void Body::set_vel(size_t idx, double val)
{
    vel[idx] = val;
}

extern "C" __device__ __host__
void Body::set_acc(size_t idx, double val)
{
    acc[idx] = val;
}

extern "C" __device__ __host__
void Body::set_color(size_t idx, double val)
{
    color[idx] = val;
}

extern "C" __device__ __host__
void Body::update_pos(size_t idx, double delta)
{
    pos[idx] += delta;
}

extern "C" __device__ __host__
void Body::update_vel(size_t idx, double delta)
{
    vel[idx] += delta;
}

extern "C" __device__ __host__
void Body::update_acc(size_t idx, double delta)
{
    acc[idx] += delta;
}

std::string Body::to_string()
{
    size_t i;
    std::ostringstream os;
    os << "(mass: " << mass << ", radius: " << radius << ", ";
    for (i = 0; i < DIMS; i++)
    {
        os << "color[" << i << "]: " << color[i] << ", ";
    }
    for (i = 0; i < DIMS; i++)
    {
        os << "pos[" << i << "]: " << pos[i] << ", ";
    }
    for (i = 0; i < DIMS; i++)
    {
        os << "vel[" << i << "]: " << vel[i] << ", ";
    }
    for (i = 0; i < DIMS - 1; i++)
    {
        os << "acc[" << i << "]: " << acc[i] << ", ";
    }
    os << "acc[" << i << "]: " << acc[i] << ")";
    return os.str();
}

