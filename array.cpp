#include "array.h"

template <typename T>
Array<T>::Array(size_t size)
{
    this->size = size;
    array = new T[size];
}

template <typename T>
size_t Array<T>::get_size()
{
    return size;   
}

template <typename T>
T Array<T>::get(size_t idx)
{
    return array[idx];
}

template <typename T>
void Array<T>::set(size_t idx, T val)
{
    array[idx] = val;
}

template <typename T>
Array<T>::~Array()
{
    delete[] array;
}
