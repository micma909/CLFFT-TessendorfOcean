#include "cl_platform.h"
#include "cl_helper.h"
#include <map>
#include <iterator>
#include <iostream>
#include <string>
CL::platform CL::platform::any_platform(){
    std::vector<CL::platform> platforms = all_platforms();
    return platforms[0];
}

CL::platform CL::platform::preferred_platform(const std::string &platform_name){
    std::vector<CL::platform> platforms = all_platforms();
    for (CL::platform &platform : platforms) {
        if (platform.name() == platform_name) {
            return platform;
        }
    }
    std::cout << "Platform \"" << platform_name << "\" not found. ";
    std::cout << "Using \"" << platforms[0].name() << "\" instead" << std::endl;
    return platforms[0];
}

std::vector<CL::platform> CL::platform::all_platforms(){
    //
    cl_uint no_platforms;
    std::vector<cl_platform_id> cl_platforms;
    std::vector<CL::platform> platforms;

    //
    no_platforms = available_platforms();
    cl_platforms.resize(no_platforms);

    //Get available platforms
    CL_ERROR(clGetPlatformIDs(no_platforms, &cl_platforms[0], nullptr));
    
    //Make CL::platform objects
    std::back_insert_iterator< std::vector<CL::platform> > back_it (platforms);
    std::copy (cl_platforms.begin(),cl_platforms.end(),back_it);
    return platforms;
}

CL::platform::platform():object<cl_platform_id>(){
    
}

CL::platform::platform(cl_platform_id platform):object<cl_platform_id>(){
    CL::object<cl_platform_id>::object_ = platform;
}

CL::platform::~platform(){

}

void CL::platform::operator=(const CL::platform &p){
    CL::object<cl_platform_id>::object_ = p.object_;
}

std::string CL::platform::name() const{
    size_t size;
    CL_ERROR(clGetPlatformInfo(handle(),
                               CL_PLATFORM_NAME,
                               0,
                               nullptr,
                               &size));
    char *name = new char[size];
    CL_ERROR(clGetPlatformInfo(handle(),
                               CL_PLATFORM_NAME,
                               size,
                               name,
                               nullptr));
    std::string str(name);
    delete [] name;
    return str;
}

cl_uint CL::platform::available_platforms(){
    cl_uint no_platforms;
    cl_int error = clGetPlatformIDs(0,
                                    nullptr,
                                    &no_platforms);
    CL_ERROR(error);
    return no_platforms;
}

void CL::platform::info(std::ostream &s) const{
    std::map<cl_platform_info, std::string > info;
    info.insert(std::pair<cl_platform_info, std::string>(CL_PLATFORM_PROFILE,"CL_PLATFORM_PROFILE"));
    info.insert(std::pair<cl_platform_info, std::string>(CL_PLATFORM_VERSION,"CL_PLATFORM_VERSION"));
    info.insert(std::pair<cl_platform_info, std::string>(CL_PLATFORM_NAME,"CL_PLATFORM_NAME"));
    info.insert(std::pair<cl_platform_info, std::string>(CL_PLATFORM_VENDOR,"CL_PLATFORM_VENDOR"));
    info.insert(std::pair<cl_platform_info, std::string>(CL_PLATFORM_EXTENSIONS,"CL_PLATFORM_EXTENSIONS"));
    
    for (std::map<cl_platform_info,std::string>::iterator it = info.begin(); it != info.end();it++) {
        //Size
        size_t size;
        CL_ERROR(clGetPlatformInfo(handle(),
                                   it->first,
                                   0,
                                   nullptr,
                                   &size));
        //string
        char *info = new char[size];
        CL_ERROR(clGetPlatformInfo(handle(),
                                   it->first,
                                   size,
                                   info,
                                   nullptr));
        s << it->second << ": " << info << std::endl;
		delete [] info;
    }
}