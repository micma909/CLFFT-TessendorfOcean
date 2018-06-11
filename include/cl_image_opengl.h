#ifndef CLWL_cl_image_opengl_h
#define CLWL_cl_image_opengl_h
#include "cl_data_opengl.h"

namespace CL{
    class image_opengl{
    private:
        //Device data
        CL::data_opengl data_;
    public:
		image_opengl();
        image_opengl(CL::context &ctx,
                     GLuint texture,
                     cl_mem_flags memflags,
                     size_t size,
                     GLenum target = GL_TEXTURE_2D,
                     GLuint mipLevel = 0);
        image_opengl(const CL::image_opengl &t);
        ~image_opengl();
        void operator=(const CL::image_opengl &k);
        CL::data data();
        
        //
        CL::event aquireGLObject(CL::command_queue &cmq);
        CL::event releaseGLObject(CL::command_queue &cmq);
        
    };
    
    //Factory methods
    //CL::image<2> image2D_rgbaf(CL::context &ctx,size_t X,size_t Y);
    //CL::image<3> image3D_rgbaf(CL::context &ctx,size_t X,size_t Y,size_t Z);
    
}

#endif
