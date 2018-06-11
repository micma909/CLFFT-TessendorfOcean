#include "cl_device.h"
#include "cl_helper.h"
#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <stdexcept>


/* Get Device Info */
//template<> cl_uint CL::device::info<cl_uint>(cl_device_info info) const;
//template<> cl_int CL::device::info<cl_int>(cl_device_info info) const;
//template<> cl_bool CL::device::info<cl_bool>(cl_device_info info) const;

template<class T>
T CL::device::info(cl_device_info info) const{
    T value;
    CL_ERROR(clGetDeviceInfo(handle(),
                             info,
                             sizeof(T),
                             &value,
                             nullptr));
    return value;
}

//Constructor
CL::device CL::device::preferred_device(const CL::platform &platform,
                                        cl_device_type device_type){
    auto devices = all_devices(platform,device_type);
    for (CL::device &d : devices) {
        if (d.available()) {
            return d;
        }
    }
    throw std::runtime_error("No available OpenCL devices could be found!");
    return nullptr;
}

CL::device CL::device::preferred_device(const CL::platform &platform,
                                        const std::string &device_name){
    auto devices =  all_devices(platform, CL_DEVICE_TYPE_ALL);
    for (CL::device &d : devices) {
        if (d.name().compare(device_name) == 0) {
            return d;
        }
    }
    return devices[0];
}

CL::device CL::device::any_gpu_device(const CL::platform &platform){
    return preferred_device(platform, CL_DEVICE_TYPE_GPU);
}
CL::device CL::device::any_avaiable_device(const CL::platform &platform){
    return all_available_devices(platform, CL_DEVICE_TYPE_ALL)[0];
}
CL::device CL::device::any_avaiable_gpu(const CL::platform &platform){
    return *all_available_devices(platform, CL_DEVICE_TYPE_GPU).rbegin();
}

CL::device CL::device::manually(const CL::platform &platform){
    
    //Print all avaiable devices
    auto devices = all_devices(platform, CL_DEVICE_TYPE_ALL);
    if (devices.size() == 0) throw std::runtime_error("No available devices!");
    
    std::cout << "Available OpenCL devices:" << std::endl;
    for (int i = 0; i <devices.size();i++) {
        std::cout << i << " " << devices[i].name() << std::endl;
    }
    std::cout << "Pick OpenCL device(0-" << devices.size()-1 << "): ";
    int d;
    do{
        std::cin >> d;
    }while (d < 0 || d > devices.size()-1);
    CL::device device = devices[d];
    std::cout << "Excellent choice sir. " << device.name() << " is selected" << std::endl;
    return device;
}

void CL::device::print_all_devices(const CL::platform &platform,std::ostream &stream){
    auto devices  = all_devices(platform, CL_DEVICE_TYPE_ALL);
    for (const CL::device &d : devices) {
        std::cout << d.name() << std::endl;
    }
}


CL::device CL::device::default_device(const CL::platform &platform){
	return preferred_device(platform, CL_DEVICE_TYPE_DEFAULT);
}

std::vector<CL::device> CL::device::all_available_devices(const CL::platform &platform,  cl_device_type device_type){
    std::vector<CL::device> devices = all_devices(platform,device_type);
    std::remove_if(devices.begin(), devices.end(), [](const CL::device &d)->bool{
        return !d.available();
    });
    return devices;
}

CL::device CL::device::supporting_extension(const CL::platform &platform, const std::vector<std::string> &extensions, cl_device_type device_type){
    std::vector<CL::device> devices = all_available_devices(platform, device_type);
    for (CL::device& d : devices) {
        std::cout << d.name() << std::endl;
        bool hasSupport = true;
        for (const std::string &extension : extensions) {
            hasSupport &= d.supports_extension(extension);
        }
        if(hasSupport){
            return d;
        }
    }
    throw std::runtime_error("No device found");
}

std::vector<CL::device> CL::device::all_devices(const CL::platform &platform,
                                                cl_device_type device_type){
    cl_uint no_devices;
    std::vector<cl_device_id> cl_devices;
    std::vector<CL::device> devices;
    
    CL_ERROR(clGetDeviceIDs(platform.handle(),
                            device_type,
                            0,
                            nullptr,
                            &no_devices));
    cl_devices.resize(no_devices);
    CL_ERROR(clGetDeviceIDs(platform.handle(),
                            device_type,
                            no_devices,
                            &cl_devices[0],
                            nullptr));
    
    std::back_insert_iterator< std::vector<CL::device> > back_it (devices);
    std::copy (cl_devices.begin(),cl_devices.end(),back_it);

    
    return devices;
}

CL::device::device():device_id_(nullptr){

}
CL::device::device(const CL::device &device):device_id_(device.handle()){
}
CL::device::device(const cl_device_id &device_id):device_id_(device_id){
}

CL::device::~device(){
    
}

void CL::device::operator=(const CL::device &p){
    device_id_ = p.device_id_;
}

cl_device_id CL::device::handle() const{
    if (device_id_ == nullptr) {
        std::cout << "Warning: Device not initialized" << std::endl;
    }
    return device_id_;
}
std::string CL::device::name() const{
    size_t size;
    CL_ERROR(clGetDeviceInfo(handle(),
                             CL_DEVICE_NAME,
                             0,
                             nullptr,
                             &size));
    //
    char *name = new char[size];
    CL_ERROR(clGetDeviceInfo(handle(),
                             CL_DEVICE_NAME,
                             size,
                             name,
                             nullptr));
    std::string str(name);
    delete [] name;
    return str;
}

bool CL::device::available() const{
    return info<cl_bool>(CL_DEVICE_AVAILABLE) == CL_TRUE;
}

cl_uint CL::device::max_clock_frequency() const{
    return info<cl_uint>(CL_DEVICE_MAX_CLOCK_FREQUENCY);
}

cl_ulong CL::device::global_mem_size() const{
    return info<cl_ulong>(CL_DEVICE_GLOBAL_MEM_SIZE);
}


cl_uint CL::device::max_compute_units() const{
    return info<cl_uint>(CL_DEVICE_MAX_COMPUTE_UNITS);
}

cl_uint CL::device::vendor_id() const{
    return info<cl_uint>(CL_DEVICE_VENDOR_ID);
}

std::string CL::device::extensions() const{
    size_t e_size;
    
    CL_ERROR(clGetDeviceInfo(handle(),
                             CL_DEVICE_EXTENSIONS,
                             0,
                             nullptr,
                             &e_size));
    
    char * e_str = new char[e_size];
    
    CL_ERROR(clGetDeviceInfo(handle(),
                             CL_DEVICE_EXTENSIONS,
                             e_size,
                             e_str,
                             nullptr));
    std::string ex(e_str);
    delete[] e_str;
    
    return ex;
}

bool CL::device::supports_extension(const std::string& extension) const {
	std::string ex = extensions();
	return ex.find(extension) != std::string::npos;
}

cl_uint CL::device::reference_count() const{
    cl_uint count;
    CL_ERROR(clGetDeviceInfo(handle(),
                             CL_DEVICE_REFERENCE_COUNT,
                             sizeof(cl_uint),
                             &count,
                             nullptr));
    return count;
}


cl_uint CL::device::available_devices(const CL::platform& platform,
                                      cl_device_type device_type){
     cl_uint no_devices;
     CL_ERROR(clGetDeviceIDs(platform.handle(), device_type, 0, nullptr, &no_devices));
     return no_devices;
}
void CL::device::info(std::ostream &s) const{
    std::map<cl_platform_info, std::string > info;
    info.insert(std::pair<cl_platform_info, std::string>(CL_DEVICE_NAME,"CL_DEVICE_NAME"));
    info.insert(std::pair<cl_platform_info, std::string>(CL_DEVICE_VENDOR,"CL_DEVICE_VENDOR"));
    info.insert(std::pair<cl_platform_info, std::string>(CL_DRIVER_VERSION,"CL_DRIVER_VERSION"));
    info.insert(std::pair<cl_platform_info, std::string>(CL_DEVICE_PROFILE,"CL_DEVICE_PROFILE"));
    info.insert(std::pair<cl_platform_info, std::string>(CL_DEVICE_VERSION,"CL_DEVICE_VERSION"));
    info.insert(std::pair<cl_platform_info, std::string>(CL_DEVICE_EXTENSIONS,"CL_DEVICE_EXTENSIONS"));
    
    for (std::map<cl_platform_info, std::string >::iterator it = info.begin(); it != info.end(); it++) {
        size_t size;
        CL_ERROR(clGetDeviceInfo(handle(), it->first, 0, nullptr, &size));
        //
        char *string = new char[size];
        CL_ERROR(clGetDeviceInfo(handle(), it->first, size, string, nullptr));
        s << it->second << " : " << string << std::endl;
		delete [] string;
    }
}





