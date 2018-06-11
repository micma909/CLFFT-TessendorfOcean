#include "cl_gl_context_apple.h"
#include "cl_helper.h"
#include "cl_external_libraries.h"
#include <algorithm>
#include <stdexcept>

CL::gl_context::gl_context(const CL::device &device,std::ostream *stream):context(nullptr){
    std::vector<cl_device_id> vec;
    vec.push_back(device.handle());
    init(vec);
}

CL::gl_context::gl_context(const std::vector<CL::device> &devices,std::ostream *stream):context(nullptr){
    std::vector<cl_device_id> vec;
    std::for_each(devices.begin(), devices.end(), [&](const CL::device &device){
        vec.push_back(device.handle());
    });
    
    init(vec);
}

void CL::gl_context::init(const std::vector<cl_device_id> &devices){
    
    // Get current CGL Context and CGL Share group
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    
    for (cl_device_id d : devices) {
        CL::device device(d);
        if (!device.supports_extension("cl_APPLE_gl_sharing")) {
            throw std::runtime_error("OpenGL sharing (cl_APPLE_gl_sharing) is not supported!");
        }
    }
    
    cl_context_properties properties[] = {CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,(cl_context_properties)kCGLShareGroup, 0};
    
    cl_int err;
    context_ = clCreateContext(properties,
                               (cl_int)devices.size(),
                               &devices[0],
                               CL::context::context_callback,
                               CL::context::stream_,
                               &err);
    CL_ERROR(err);
}

std::vector<std::string> CL::gl_context::requiredExtensions(){
    std::string e = "cl_APPLE_gl_sharing";
    std::vector<std::string> v;
    v.push_back(e);
    return v;
}
