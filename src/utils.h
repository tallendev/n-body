#ifndef UTILS_H
#define UTILS_H

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }

// Ref: http://stackoverflow.com/a/14038590
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
    if (code != cudaSuccess) 
    {
        fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
        if (abort) exit(code);
    }
}

#endif
