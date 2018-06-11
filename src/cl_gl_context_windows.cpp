#include "cl_gl_context_windows.h"
#include "cl_helper.h"
#include <algorithm>

CL::gl_context::gl_context(const CL::device &device, const CL::platform& platform, std::ostream *stream){
	std::vector<cl_device_id> vec;
	vec.push_back(device.handle());
	init(vec, platform.handle());
}

CL::gl_context::gl_context(const std::vector<CL::device> &devices, const CL::platform& platform, std::ostream *stream){
	std::vector<cl_device_id> vec;
	std::for_each(devices.begin(), devices.end(), [&](const CL::device &device){
		vec.push_back(device.handle());
	});

	init(vec, platform.handle());
}

void CL::gl_context::init(const std::vector<cl_device_id> &devices, const cl_platform_id platform){
	cl_int err;

	std::string exstr("cl_khr_gl_sharing");
	for (cl_device_id d : devices){
		CL::device dev(d);
		if (!dev.supports_extension(exstr)){
			throw std::runtime_error("Extension not found: cl_khr_gl_sharing");
		}
	}

	cl_context_properties props[] =
	{
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
		0
	};

	context_ = clCreateContext(props, 
							   (cl_uint)devices.size(),
							   &devices[0], 
						       CL::context::context_callback, 
							   stream_, 
							   &err);
								 
	CL_ERROR(err);
}