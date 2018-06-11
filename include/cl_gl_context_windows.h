#ifndef clLib_cl_gl_contextWindows_h
#define clLib_cl_gl_contextWindows_h
#include <iostream>
#include "cl_context.h"
#include "cl_external_libraries.h"

namespace CL{
	class gl_context : public context {
	private: 
		void init(const std::vector<cl_device_id> &devices, const cl_platform_id platform );
	public:
		gl_context() :context(){}
		gl_context(const cl_context &c) :context(c){}
		gl_context(const CL::gl_context &c) :context(c){}
		gl_context(const CL::device &device, const CL::platform &platform, std::ostream *stream = &std::cout);
		gl_context(const std::vector<CL::device> &devices, const CL::platform& platform, std::ostream *stream = &std::cout);
	};

}


#endif
