#include "cl_program.h"
#include "cl_helper.h"
#include "cl_exception.h"

#include <vector>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <mutex>



CL::program::program():program_(nullptr){

}
CL::program::program(const cl_program &program):program_(program){
    if (program_)  CL_ERROR(clRetainProgram(program_));
}

CL::program::program(const CL::program &program):program_(program.handle()){
    if (program_)  CL_ERROR(clRetainProgram(program_));
}

CL::program CL::program::file(CL::context &context,
                     CL::device &device,
                     const std::string &source_file,
                     const std::string &options){
    CL::program program;
    //load soure
    program.build(context, device, source(source_file),options);
    return program;
}

CL::program CL::program::source(CL::context &context,
                                CL::device &device,
                                const std::string &source_file,
                                const std::string &options){
    CL::program program;
    //load soure
    program.build(context, device, source_file, options);
    return program;
}

CL::program::~program(){
    if (program_) CL_ERROR(clReleaseProgram(program_));
}

void CL::program::operator=(const CL::program &p){
    if (program_) CL_ERROR(clReleaseProgram(program_));
    program_ = p.program_;
    if (program_) CL_ERROR(clRetainProgram(program_));
}

std::string CL::program::source(const std::string &src_file){
    std::ifstream ifs(src_file.c_str());
    if (!ifs){
        std::stringstream ss;
        ss << "File " << src_file << " not found";
        throw std::runtime_error(ss.str());
    }
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    return content;
}

void CL::program::build_for_devices(const cl_context &context,
                                    const std::vector<cl_device_id> &devices,
                                    const std::string &source,
                                    const std::string &options){
    cl_int error;

    /* Load source code */
    const char * c_src = source.c_str();
	size_t length[] = { strlen(c_src) };
    /*Create program on context*/

    program_ = clCreateProgramWithSource(context,
                                         1,
                                         (const char **) & c_src,
                                         length,
                                         &error);
    CL_ERROR(error);
    
    /*Build program for devices*/
    try {
        CL_ERROR(clBuildProgram(program_,
                                (cl_uint)devices.size(),
                                &devices[0],
                                nullptr,
                                nullptr,
                                nullptr));
    } catch (CL::exception &e) {
        if (e.error_code() == CL_BUILD_PROGRAM_FAILURE) {
            //Add additional info
            for (int i = 0; i < devices.size(); i++) {
                std::stringstream ss;
                CL::device d(devices[i]);
                ss << "BUILD LOG FOR " << d.name() << std::endl;
                e.addAdditionalInfo(build_log(d),ss.str());
            } 
        }
        throw; //Re-throw
    }

}

void CL::program::build(const CL::context &context,
                        const CL::device &device,
                        const std::string &source,
                        const std::string &options){
    std::vector<cl_device_id> vec;
    vec.push_back(device.handle());
    build_for_devices(context.handle(),vec,source,options);
}
void CL::program::build(const CL::context &context,
                        const std::vector<CL::device> &devices,
                        const std::string &source,
                        const std::string &options){
    std::vector<cl_device_id> vec;
    std::for_each(devices.begin(), devices.end(), [&](const CL::device &device){
        vec.push_back(device.handle());
    });
    build_for_devices(context.handle(),vec,source,options);
}

std::string CL::program::build_log(const CL::device &d) const{
    std::string s;
    try{
        size_t size;
        CL_ERROR(clGetProgramBuildInfo(program_, d.handle(), CL_PROGRAM_BUILD_LOG, 0, nullptr, &size));
        size_t chars = size/sizeof(char);
        char *msg = new char[chars];
        CL_ERROR(clGetProgramBuildInfo(program_, d.handle(), CL_PROGRAM_BUILD_LOG, size, msg, nullptr));
        s.resize(chars);
        std::copy(msg, msg+chars, s.begin());
        delete [] msg;
    }catch(...){
        return "";
    }
    return s;
}

cl_program CL::program::handle() const{
    return program_;
}