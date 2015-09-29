#include "body.h"

Body::Body() : Body::Body(0, 0) {}

Body::Body(double mass, double radius)
{
    this->mass = mass;
    this->radius = radius;
    for (size_t i = 0; i < DIMS; i++)
    {
            
    }
    for (size_t i = 0; i < DIMS; i++)
    {
    }
}


