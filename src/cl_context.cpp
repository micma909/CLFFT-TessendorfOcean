#include "cl_context.h"
#include "cl_helper.h"
#include <mutex>
#include <algorithm>

void CL_CALLBACK  CL::context::context_callback(const char *errinfo,
                                                const void *private_info,
                                                size_t cb,
                                                void *user_data){
    if (user_data) {
        static std::mutex mutex;
        mutex.lock();
        std::ostream *stream = (std::ostream *)user_data;
        *stream << "-------- Context Callback ---------" << std::endl;
        *stream << "ERROR INFO: " << errinfo << std::endl;
        *stream << "--------------- END ---------------" << std::endl;
        mutex.unlock();
    }
}

// ----
CL::context::context():context_(nullptr),stream_(nullptr){
    
}
CL::context::context(const cl_context &context):context_(context),stream_(nullptr){
    if(context_) CL_ERROR(clRetainContext(context_));
}

CL::context::context(const CL::context &context):context_(context.handle()),stream_(context.stream_){
    if(context_) CL_ERROR(clRetainContext(context_));
}

CL::context::context(const CL::device &device,std::ostream *stream):context_(nullptr),stream_(stream){
    std::vector<cl_device_id> vec;
    vec.push_back(device.handle());
    init(vec);
}

CL::context::context(const std::vector<CL::device> &devices,
                     std::ostream *stream):context_(nullptr),stream_(stream){
    std::vector<cl_device_id> vec;
    std::for_each(devices.begin(), devices.end(), [&](const CL::device &device){
        vec.push_back(device.handle());
    });
    
    init(vec);
}

CL::context::~context(){
    if (context_) CL_ERROR(clReleaseContext(context_));
}

std::vector<CL::device> CL::context::getDevices() const{
    
    // Check the number of devices accociated...
    size_t device_list_size;
    CL_ERROR(clGetContextInfo(context_,
                              CL_CONTEXT_DEVICES,
                              0,
                              NULL,
                              &device_list_size));
    enum{
        MAX_DEVICES = 16
    };
    

    
    // Do we even have any devices?
    if(!device_list_size) throw std::runtime_error("No accociated devices found!");
    if(device_list_size > MAX_DEVICES) throw std::runtime_error("Too many devices found (wtf)");

    // Get the devices
    cl_device_id devices[MAX_DEVICES];
    CL_ERROR(clGetContextInfo(context_,
                              CL_CONTEXT_DEVICES,
                              device_list_size,
                              devices,
                              NULL));
    
    std::vector<CL::device> vdevices;
    for (int i = 0; i < device_list_size; ++i) {
        vdevices.push_back(devices[i]);
    }
    return std::move(vdevices);
}


void CL::context::operator=(const CL::context &c){
    if (context_) CL_ERROR(clReleaseContext(context_));
    
    context_ = c.context_;
    stream_ = c.stream_;
    if (context_) CL_ERROR(clRetainContext(context_));
}

void CL::context::init(const std::vector<cl_device_id> &devices){

    cl_int error;
    //Create context
    context_ = clCreateContext(nullptr,
                               (cl_uint)devices.size(),
                               &devices[0],
                               stream_ ? context_callback : nullptr ,
                               stream_,
                               &error);
    CL_ERROR(error);
}

cl_uint CL::context::reference_count() const{
    
    cl_uint count;
    CL_ERROR(clGetContextInfo(context_,
                              CL_CONTEXT_REFERENCE_COUNT,
                              sizeof(cl_uint),
                              &count,
                              nullptr));

    return count;
}

cl_context CL::context::handle() const{
    if (context_ == nullptr) {
        std::cout << "Warning: Context not initialized" << std::endl;
    }
    return context_;
}

