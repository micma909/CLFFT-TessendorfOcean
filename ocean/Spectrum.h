#ifndef __Ocean__Spectrum__
#define __Ocean__Spectrum__

#include <iostream>
#if __APPLE__
#include <gl/glew.h>
#include <OpenCL/OpenCL.h>
#else
#include <gl/glew.h>
#include <CL/cl.hpp>
#endif
namespace Spectrum{
    void phillips_wave(int n,float L,const float wind[2],cl_float *spectrum);
    void random_phase(int n,cl_float *phase);
}
#endif /* defined(__Ocean__Spectrum__) */
