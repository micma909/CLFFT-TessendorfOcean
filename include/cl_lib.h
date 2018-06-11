#ifndef clUtilLib_cl_wrapper_h
#define clUtilLib_cl_wrapper_h

#include "cl_helper.h"
#include "cl_exception.h"

#include "cl_platform.h"

#include "cl_device.h"

#include "cl_context.h"

#include "cl_program.h"

#include "cl_kernel.h"

#include "cl_data.h"
#include "cl_mem.h"
#include "cl_shared_data.h"
#include "cl_data_opengl.h"

#include "cl_image.h"
#include "cl_image_opengl.h"


#include "cl_command_queue.h"

#include "cl_event.h"

#if defined (__APPLE__) || defined(MACOSX)
#include "cl_gl_context_apple.h"
#elif defined(_WIN32) || defined(__WINDOWS__)
#include "cl_gl_context_windows.h"
#elif defined(__gnu_linux__)

#endif

#endif
