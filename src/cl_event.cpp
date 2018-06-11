#include "cl_event.h"
#include "cl_helper.h"
#include <mutex>

//Callback

void CL_CALLBACK CL::event::callback(cl_event event, cl_int status, void *stream_ptr){
    event_callback_t *user_data = (event_callback_t*)stream_ptr;
    if (user_data != nullptr && user_data->stream != nullptr) {
        static std::mutex mutex;
        mutex.lock();
        std::ostream *stream = user_data->stream;
        cl_command_type type;
        CL_ERROR(clGetEventInfo (event,
                                 CL_EVENT_COMMAND_TYPE,
                                 sizeof(cl_command_type),
                                 &type,
                                 nullptr));
        *stream << "---------- Event Callback ---------" << std::endl;
        *stream << "Event \"" << user_data->event_name << "\" status: " <<  status_to_string(status) << std::endl;
        *stream << "--------------- END ---------------" << std::endl;
        mutex.unlock();
    }
}
//Protected
std::string CL::event::status_to_string(cl_int status) {
#define RET_NAME(type) case type: return #type; break;
    switch (status) {
            RET_NAME(CL_QUEUED)
            RET_NAME(CL_SUBMITTED)
            RET_NAME(CL_RUNNING)
            RET_NAME(CL_COMPLETE)
        default:
            return "Unknown command type name";
            break;
    }
}

std::string CL::event::command_to_string(cl_command_type type){
#define RET_NAME(type) case type: return #type; break;
    switch (type) {
            RET_NAME(CL_COMMAND_NDRANGE_KERNEL)
            RET_NAME(CL_COMMAND_TASK)
            RET_NAME(CL_COMMAND_NATIVE_KERNEL)
            RET_NAME(CL_COMMAND_READ_BUFFER)
            RET_NAME(CL_COMMAND_WRITE_BUFFER)
            RET_NAME(CL_COMMAND_COPY_BUFFER)
            RET_NAME(CL_COMMAND_READ_IMAGE)
            RET_NAME(CL_COMMAND_WRITE_IMAGE)
            RET_NAME(CL_COMMAND_COPY_IMAGE)
            RET_NAME(CL_COMMAND_COPY_BUFFER_TO_IMAGE)
            RET_NAME(CL_COMMAND_COPY_IMAGE_TO_BUFFER)
            RET_NAME(CL_COMMAND_MAP_BUFFER)
            RET_NAME(CL_COMMAND_MAP_IMAGE)
            RET_NAME(CL_COMMAND_UNMAP_MEM_OBJECT)
            RET_NAME(CL_COMMAND_MARKER)
            RET_NAME(CL_COMMAND_ACQUIRE_GL_OBJECTS)
            RET_NAME(CL_COMMAND_RELEASE_GL_OBJECTS)
        default:
            return "Unknown command type name";
            break;
    }
}


//
CL::event::event():event_(nullptr){
    
}

CL::event::event(cl_event event):event_(event){
    if(event_) CL_ERROR(clRetainEvent(event_));
}

CL::event::event(CL::event &event):event_(event.event_){
    if(event_) CL_ERROR(clRetainEvent(event_));
}

CL::event::event(CL::event &&event):event_(event.event_){
    if(event_) CL_ERROR(clRetainEvent(event_));
}

CL::event::~event(){
    if(event_) CL_ERROR(clReleaseEvent(event_));
}

void CL::event::operator=(const CL::event &c){
    if(event_) CL_ERROR(clReleaseEvent(event_));
    event_ = c.event_;
    if(event_) CL_ERROR(clRetainEvent(event_));
}

cl_command_type CL::event::command_type() const{
    cl_command_type type;
    CL_ERROR(clGetEventInfo (event_,
                             CL_EVENT_COMMAND_TYPE,
                             sizeof(cl_command_type),
                             &type,
                             nullptr));
    return type;
}

std::string CL::event::command_type_name() const{
    cl_command_type type = command_type();
    return command_to_string(type);
}

cl_int CL::event::event_status() const{
    cl_int status;
    CL_ERROR(clGetEventInfo (event_,
                             CL_EVENT_COMMAND_EXECUTION_STATUS,
                             sizeof(cl_int),
                             &status,
                             nullptr));
    return status;
}

std::string CL::event::event_status_name() const{
    cl_int status = event_status();
    return status_to_string(status);
}

void CL::event::enable_callback(const std::string &event_name,std::ostream *stream){
    if(event_){
        user_data.event_name = event_name;
        user_data.stream = stream;
        CL_ERROR(clSetEventCallback(event_, 1, callback, &user_data));
    }
}


void CL::event::wait() {
    if(event_){
        CL_ERROR(clWaitForEvents(1, &event_));
    }
}

cl_event CL::event::handle() const{
    return event_;
}