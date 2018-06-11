#ifndef clLib_cl_external_libraries_h
#define clLib_cl_external_libraries_h

#if defined (__APPLE__) || defined(MACOSX)
#include "cl_external_apple.h"
#elif defined(_WIN32) || defined(__WINDOWS__)
#include "cl_external_windows.h"
#elif defined(__gnu_linux__)
#include "cl_external_linux.h"
#endif

#endif
