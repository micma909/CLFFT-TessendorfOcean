#include "cl_data_opengl.h"
#include "cl_helper.h"
#include "cl_event.h"


CL::data_opengl::data_opengl(const CL::context &context,
                           cl_mem_flags flags,
                           cl_GLuint gl_obj,
                           size_t size):data(){
    size_ = size;
    cl_int error;
    data_ = clCreateFromGLBuffer(context.handle(),
                                flags,
                                gl_obj,
                                &error);
    CL_ERROR(error);
}

CL::event CL::data_opengl::aquireGLObject(CL::command_queue &cmq){
    // Aquire ownership of GL vbo for OpenCL Image
    cl_event event;
    CL_ERROR(clEnqueueAcquireGLObjects(cmq.handle(),
                                       1,
                                       &data_,
                                       0,
                                       nullptr,
                                       &event));
    
    return event;
}

CL::event CL::data_opengl::releaseGLObject(CL::command_queue &cmq){
    // Release ownership of GL vbo for OpenCL Image
    cl_event event;
    CL_ERROR(clEnqueueReleaseGLObjects(cmq.handle(),
                                       1,
                                       &data_,
                                       0,
                                       nullptr,
                                       &event));
    return event;
}
