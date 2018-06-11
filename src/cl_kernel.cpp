#include "cl_kernel.h"
#include "cl_helper.h"
#include "cl_event.h"
#include "cl_data.h"
#include "cl_shared_data.h"
#include "cl_data_opengl.h"


CL::kernel::kernel():kernel_(nullptr){
    
}

CL::kernel::kernel(const cl_kernel &kernel):kernel_(kernel){
    CL_ERROR(clRetainKernel(kernel_));
}
CL::kernel::kernel(const CL::kernel &kernel):kernel_(kernel.handle()){
    CL_ERROR(clRetainKernel(kernel_));
}

CL::kernel::kernel(CL::program program, const std::string &name):kernel_(nullptr),name_(name){
    cl_int error;
    kernel_ = clCreateKernel(program.handle(),name_.c_str(), &error);
    clRetainKernel(kernel_); //Both cl_program and cl_kernel are responsible for releasing the kernel
    CL_ERROR(error);
}

CL::kernel::~kernel(){
    if(kernel_ != nullptr) CL_ERROR(clReleaseKernel(kernel_));
}

void CL::kernel::operator=(const CL::kernel &k){
    if (kernel_) CL_ERROR(clReleaseKernel(kernel_));
    kernel_ = k.kernel_;
    name_ = k.name_;
    if (kernel_) CL_ERROR(clRetainKernel(kernel_));
}

cl_kernel CL::kernel::handle() const{
    return kernel_;
}

template<class T>
T CL::kernel::work_group_info(const CL::device &d, cl_kernel_work_group_info info) const{
    T value;
    CL_ERROR(clGetKernelWorkGroupInfo(handle(),
                                      d.handle(),
                                      info,
                                      sizeof(T),
                                      &value,
                                      NULL));
    return value;
}

cl_ulong CL::kernel::private_mem_size(const CL::device &d) const{
	return work_group_info<cl_ulong>(d, CL_KERNEL_PRIVATE_MEM_SIZE);
}

CL::event CL::kernel::async_execute1D(const CL::command_queue &queue,
                           size_t work_group_dim,
                           size_t global_dim){
    cl_event event;
    CL_ERROR(clEnqueueNDRangeKernel(queue.handle(),
                                    kernel_,
                                    (cl_uint)1,
                                    nullptr,
                                    &global_dim,
                                    &work_group_dim,
                                    0,
                                    nullptr,
                                    &event));
    return event;
}

CL::event CL::kernel::async_execute2D(const CL::command_queue &queue,
                           size_t work_group_dim_x,
                           size_t work_group_dim_y,
                           size_t global_dim_size_x,
                           size_t global_dim_size_y){
    cl_event event;

    size_t work_group_dim[2] = {work_group_dim_x,work_group_dim_y};
    size_t global_dim[2] =  {global_dim_size_x,global_dim_size_y};
    
    CL_ERROR(clEnqueueNDRangeKernel(queue.handle(),
                                    kernel_,
                                    (cl_uint)2,
                                    nullptr,
                                    global_dim,
                                    work_group_dim,
                                    0,
                                    nullptr,
                                    &event));
    return event;
}



CL::event CL::kernel::async_execute3D(const CL::command_queue &queue,
                           size_t work_group_dim_x,
                           size_t work_group_dim_y,
                           size_t work_group_dim_z,
                           size_t global_dim_size_x,
                           size_t global_dim_size_y,
                           size_t global_dim_size_z){
    cl_event event;

    size_t work_group_dim[3] = {work_group_dim_x,work_group_dim_y,work_group_dim_z};
    size_t global_dim[3] =  {global_dim_size_x,global_dim_size_y,global_dim_size_z};
    CL_ERROR(clEnqueueNDRangeKernel(queue.handle(),
                                    kernel_,
                                    (cl_uint)3,
                                    nullptr,
                                    global_dim,
                                    work_group_dim,
                                    0,
                                    nullptr,
                                    &event));
    return event;
}

void CL::kernel::sync_execute1D(const CL::command_queue &queue,
                                size_t work_group_dim,
                                size_t global_dim){
    CL::event e = async_execute1D(queue, work_group_dim, global_dim);
    e.wait();
}

void CL::kernel::sync_execute2D(const CL::command_queue &queue,
                                size_t work_group_dim_x,
                                size_t work_group_dim_y,
                                size_t global_dim_size_x,
                                size_t global_dim_size_y){
    CL::event e = async_execute2D(queue, work_group_dim_x,work_group_dim_y, global_dim_size_x,global_dim_size_y);
    e.wait();
}

void CL::kernel::sync_execute3D(const CL::command_queue &queue,
                                      size_t work_group_dim_x,
                                      size_t work_group_dim_y,
                                      size_t work_group_dim_z,
                                      size_t global_dim_size_x,
                                      size_t global_dim_size_y,
                                      size_t global_dim_size_z){
    CL::event e = async_execute3D(queue,work_group_dim_x,
                                  work_group_dim_y,
                                  work_group_dim_z,
                                  global_dim_size_x,
                                  global_dim_size_y,
                                  global_dim_size_z);
    e.wait();
}



// ---- setArg

void CL::kernel::setLocalArg(int i,size_t size){
    CL_ERROR(clSetKernelArg(kernel_,
                            i,
                            size,
                            NULL));
}

template<typename T>
void CL::kernel::setArg(int i,const T &buffer){
    CL_ERROR(clSetKernelArg(kernel_,
                            i,
                            sizeof(T),
                            &buffer));
}

template void CL::kernel::setArg<cl_char>(int i,const cl_char&);
template void CL::kernel::setArg<cl_uchar>(int i,const cl_uchar&);
template void CL::kernel::setArg<cl_short>(int i,const cl_short&);
template void CL::kernel::setArg<cl_ushort>(int i,const cl_ushort&);
template void CL::kernel::setArg<cl_int>(int i,const cl_int&);
template void CL::kernel::setArg<cl_uint>(int i,const cl_uint&);
template void CL::kernel::setArg<cl_long>(int i,const cl_long&);
template void CL::kernel::setArg<cl_ulong>(int i,const cl_ulong&);
//template void CL::kernel::setArg<cl_half>(int i,const cl_half&);
template void CL::kernel::setArg<cl_float>(int i,const cl_float&);
template void CL::kernel::setArg<cl_float4>(int i,const cl_float4&);
template void CL::kernel::setArg<cl_double>(int i,const cl_double&);
template void CL::kernel::setArg<cl_mem>(int i,const cl_mem&);
// ---
