#ifndef BODY_H
#define BODY_H

#include <array>
#include <cstddef>


class Body
{
    public:
        static const size_t X = 0;
        static const size_t Y = 1;
        static const size_t Z = 2;
        static const size_t DIMS = 3;

        Body();
        Body(double mass, double radius);
        void set_mass(double mass);
        void set_radius(double radius);
        double get_pos(size_t idx);
        double get_vel(size_t idx);
        double get_acc(size_t idx);
        void set_pos(size_t idx, double val);
        void set_vel(size_t idx, double val);
        void set_acc(size_t idx, double val);
        void update_pos(size_t idx, double delta);
        void update_vel(size_t idx, double delta);
        void update_acc(size_t idx, double delta);

    private:
        double mass;
        double radius;
        std::array<double, DIMS> acc;
        std::array<double, DIMS> vel;
        std::array<double, DIMS> pos;
};


#endif
