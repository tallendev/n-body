#ifndef ARRAY_H
#define ARRAY_H

#include <cstddef>

template <class T>
class Array
{
    public:
        Array(size_t size);
        ~Array();
        T get(size_t idx);
        void set(size_t idx, T val);
        size_t get_size();
    private:
        T* array;
        size_t size;
};

#endif
