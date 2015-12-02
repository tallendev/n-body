#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cuda_runtime_api.h>
#include <glew.h>
#include <GL/gl.h>


static char* readShaderSource(const char*);
GLuint InitShader(const char* vShaderFile, const char* fShaderFile);

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
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
