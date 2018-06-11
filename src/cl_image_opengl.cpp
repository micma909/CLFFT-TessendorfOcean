#include "cl_image_opengl.h"
#include "cl_helper.h"
#include "cl_event.h"
#include "cl_external_libraries.h"

CL::image_opengl::image_opengl():data_(){
}


CL::image_opengl::image_opengl(CL::context &ctx,
                                  GLuint texture,
                                  cl_mem_flags memflags,
                                  size_t size,
                                GLenum target,
                                GLuint mipLevel){

    cl_int error;
   cl_mem mem =  clCreateFromGLTexture(ctx.handle()      /* context */,
                                       memflags    /* flags */,
                                       target       /* target */,
                                       mipLevel        /* miplevel */,
                                       texture       /* texture */,
                                       &error        /* errcode_ret */);
    
    CL_ERROR(error);
    data_ = CL::data(mem,size);
}

CL::image_opengl::image_opengl(const CL::image_opengl &t):data_(t.data_){

}

CL::image_opengl::~image_opengl(){
    
}

void CL::image_opengl::operator=(const CL::image_opengl &k){
    data_ = k.data_;
}

CL::data CL::image_opengl::data(){
    return data_;
}

//

CL::event CL::image_opengl::aquireGLObject(CL::command_queue &cmq){
    return data_.aquireGLObject(cmq);
}

CL::event CL::image_opengl::releaseGLObject(CL::command_queue &cmq){
    return data_.releaseGLObject(cmq);
}
