#ifndef __clLib__cl_data_opengl__
#define __clLib__cl_data_opengl__

#include <iostream>
#include "cl_data.h"
#include "cl_event.h"

namespace CL{
    class data_opengl : public CL::data {

    public:

        data_opengl():data(){}
        data_opengl(const cl_mem &m,size_t size):data(m,size){}
        data_opengl(const CL::data &m):data(m){}
        
        data_opengl(const CL::context &context):data(context){}
        data_opengl(const CL::context &context,
                   cl_mem_flags flags,
                   cl_GLuint bufobj,
                   size_t size);
        ~data_opengl(){}

        void operator=(const CL::data &k){
            data::operator=(k);
        }
        
        //
        CL::event aquireGLObject(CL::command_queue &cmq);
        CL::event releaseGLObject(CL::command_queue &cmq);

    };
}
#endif /* defined(__clLib__cl_data_opengl__) */
