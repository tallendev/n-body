#include "body.h"
#include <sstream>
#include <string>
#include <iostream>

Body::Body() : Body::Body(5, 5) {}

Body::Body(double mass, double radius)
{
    this->mass = mass;
    this->radius = radius;
    body = gluNewQuadric();

    gluQuadricDrawStyle(body, GLU_FILL);
    gluQuadricNormals(body, GLU_SMOOTH);
    gluQuadricOrientation(body, GLU_OUTSIDE);
    gluQuadricTexture(body, GL_TRUE);

//    gluQuadricDrawStyle(body,GLU_FILL);
//    gluQuadricNormals(body,GLU_FLAT);
    for (size_t i = 0; i < DIMS; i++)
    {
        set_pos(i, 0);                 
        set_vel(i, 0);                 
        set_acc(i, 0);                 
        set_color(i, 1.0);
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

void Body::set_color(size_t idx, double val)
{
    color[idx] = val;
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

void Body::render()
{
    glColor3f(color[0], color[1], color[2]);
    glTranslated(pos[X], pos[Y], pos[Z]);
    gluSphere(body, radius, slices, stacks); 
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
