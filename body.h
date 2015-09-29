#ifndef BODY_H
#define BODY_H

#include <cstddef>

#include "array.h"

class Body
{
    public:
        static const size_t X = 0;
        static const size_t Y = 1;
        static const size_t Z = 2;
        static const size_t DIMS = 3;

        Body();
        Body(double mass, double radius);
        void set_mass();
        void set_radius();
        void get_pos();
        void get_accel();
        void set_pos();
        void set_vel();
        void set_accel();
        void update_pos();
        void update_vel();
        void update_accel();

    private:
        double mass;
        double radius;
        Array<double> acceleration(DIMS);
        Array<double> velocity(DIMS);
        Array<double> position(DIMS);
};


#endif
