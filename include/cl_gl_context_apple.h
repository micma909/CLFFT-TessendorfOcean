#ifndef __clLib__cl_gl_context__
#define __clLib__cl_gl_context__

#include <iostream>
#include "cl_context.h"

namespace CL{
    class gl_context : public context {
    private:
        void init(const std::vector<cl_device_id> &devices);
    public:
        gl_context():context(){}
        gl_context(const cl_context &c):context(c){}
        gl_context(const CL::gl_context &c):context(c){}
        gl_context(const CL::device &device,std::ostream *stream = &std::cout);
        gl_context(const std::vector<CL::device> &devices,std::ostream *stream = &std::cout);
        static std::vector<std::string> requiredExtensions();
    };
}
#endif /* defined(__clLib__cl_gl_context__) */

