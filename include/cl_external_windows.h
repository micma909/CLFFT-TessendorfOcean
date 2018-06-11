ifndef clLib_cl_external_windows_h
#define clLib_cl_external_windows_h

/**
 @brief
 Headers for the following libraries needs to be included:
 * glew
 * OpenGL
 * OpenCL
 */
#include <gl/glew.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>
#define __CL_ENABLE_EXCEPTIONS
#include "CL/cl.hpp"
#endif
