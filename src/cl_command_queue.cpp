#include "cl_command_queue.h"
#include "cl_helper.h"

CL::command_queue::command_queue():command_queue_(nullptr){
    
}

CL::command_queue::command_queue(const cl_command_queue cq):command_queue_(cq){
    if (command_queue_)  CL_ERROR(clRetainCommandQueue(command_queue_));
}

CL::command_queue::command_queue(const CL::command_queue &cq):command_queue_(cq.command_queue_){
    if (command_queue_)  CL_ERROR(clRetainCommandQueue(command_queue_));
}

CL::command_queue::command_queue(const CL::context &context,
                                 const CL::device &device,
                                 bool profilingEnabled):command_queue_(nullptr){
    cl_int error;
    command_queue_ = clCreateCommandQueue(context.handle(), device.handle(), profilingEnabled ? CL_QUEUE_PROFILING_ENABLE : 0, &error);
    CL_ERROR(error);
}

CL::command_queue::~command_queue(){
    if (command_queue_) CL_ERROR(clReleaseCommandQueue(command_queue_));
}

void CL::command_queue::operator=(const CL::command_queue &c){
    if (command_queue_) CL_ERROR(clReleaseCommandQueue(command_queue_));
    command_queue_ = c.command_queue_;
    if (command_queue_) CL_ERROR(clRetainCommandQueue(command_queue_));
}

void CL::command_queue::wait() const{
    CL_ERROR(clFinish(handle()));
}

cl_command_queue CL::command_queue::handle() const{
    if (command_queue_ == nullptr) {
        std::cout << "Warning: Command_Queue not initialized" << std::endl;
    }
    return command_queue_;
}

cl_command_queue& CL::command_queue::handle(){
    if (command_queue_ == nullptr) {
        std::cout << "Warning: Device not initialized" << std::endl;
    }
    return command_queue_;
}