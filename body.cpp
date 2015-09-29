#include "body.h"

Body::Body() : Body::Body(1, 1) {}

Body::Body(double mass, double radius)
{
    this->mass = mass;
    this->radius = radius;
    for (size_t i = 0; i < DIMS; i++)
    {
        set_pos(i, 0);                 
        set_vel(i, 0);                 
        set_acc(i, 0);                 
    }
}

void Body::set_mass(double mass)
{
    this->mass = mass;
}

void Body::set_radius(double radius)
{
    this->radius = radius;
}

double Body::get_pos(size_t idx)
{
    return pos[idx];
}

double Body::get_vel(size_t idx)
{
    return vel[idx];
}

double Body::get_acc(size_t idx)
{
    return acc[idx];
}


void Body::set_pos(size_t idx, double val)
{
    pos[idx] = val;
}

void Body::set_vel(size_t idx, double val)
{
    vel[idx] = val;
}

void Body::set_acc(size_t idx, double val)
{
    acc[idx] = val;
}

void Body::update_pos(size_t idx, double delta)
{
    pos[idx] += delta;
}

void Body::update_vel(size_t idx, double delta)
{
    vel[idx] += delta;
}

void Body::update_acc(size_t idx, double delta)
{
    acc[idx] += delta;
}

