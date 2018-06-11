#include "cl_data.h"
#include "cl_helper.h"
#include "cl_exception.h"


CL::data::data():data_(nullptr),size_(0){
}
CL::data::data(const cl_mem &data,size_t size):data_(data),size_(size){
    CL_ERROR(clRetainMemObject(data_));
}

CL::data::data(const CL::data &data):data_(data.handle()),size_(data.size_){
    CL_ERROR(clRetainMemObject(data_));
}

CL::data::data(const CL::context &context,
             cl_mem_flags flags,
             size_t size):data_(nullptr),size_(size){
    cl_int error;
    data_ = clCreateBuffer(context.handle(),
                          flags,
                          size_,
                          nullptr,
                          &error);
	try{
		CL_ERROR(error);
	}
	catch (CL::exception &e){
		if (e.error_code() == CL_INVALID_BUFFER_SIZE){
			std::stringstream ss;
			ss << size_;
			e.addAdditionalInfo(ss.str(), "BufferSize: ");
		}
		throw e;
	}
  
}

CL::data::~data(){
    //std::cout << "referenceCount()" << referenceCount() << std::endl;
    if (data_ != nullptr) CL_ERROR(clReleaseMemObject(data_));
}

void CL::data::operator=(const CL::data &k){
    if (data_) CL_ERROR(clReleaseMemObject(data_));
    data_ = k.data_;
    size_ = k.size_;
    if (data_) CL_ERROR(clRetainMemObject(data_));
}

size_t CL::data::size() const{
    return size_;
}

cl_mem CL::data::handle() const{
    return data_;
}

cl_mem& CL::data::handle(){
    return data_;
}

//

template<class T>
T CL::data::info(cl_mem_info info) const{
    T t;
    CL_ERROR(clGetMemObjectInfo(data_,
                                info,
                                sizeof(T),
                                &t, NULL));
    return t;
}

size_t CL::data::mem_size() const{
    return info<size_t>(CL_MEM_SIZE);
}

// Read
template<class T>
CL::event CL::data::read(const CL::command_queue &queue, T *host) const{
    cl_event event;
    CL_ERROR(clEnqueueReadBuffer(queue.handle(),
                                 data_,
                                 CL_FALSE,
                                 0,
                                 size_,
                                 host,
                                 0,
                                 nullptr,
                                 &event));
    return event;
}

template<class T>
void CL::data::read(const CL::command_queue &queue, T *host,cl_bool blocking) const{
    CL_ERROR(clEnqueueReadBuffer(queue.handle(),
                                 data_,
                                 blocking,
                                 0,
                                 size_,
                                 host,
                                 0,
                                 nullptr,
                                 nullptr));
}

//Explicit Instantiation read
template CL::event CL::data::read<cl_uint>(const CL::command_queue &queue,cl_uint *host) const;
template CL::event CL::data::read<cl_int>(const CL::command_queue &queue,cl_int *host) const;
template CL::event CL::data::read<cl_float>(const CL::command_queue &queue,cl_float *host) const;
template CL::event CL::data::read<cl_float2>(const CL::command_queue &queue,cl_float2 *host) const;

//Explicit Instantiation read
template void CL::data::read<cl_uint>(const CL::command_queue &queue,cl_uint *host,cl_bool blocking) const;
template void CL::data::read<cl_int>(const CL::command_queue &queue,cl_int *host,cl_bool blocking) const;
template void CL::data::read<cl_float>(const CL::command_queue &queue,cl_float *host,cl_bool blocking) const;
template void CL::data::read<cl_float2>(const CL::command_queue &queue,cl_float2 *host,cl_bool blocking) const;

// Write
template<class T>
CL::event CL::data::write(const CL::command_queue &queue, const T *host){
    cl_event event;
    CL_ERROR(clEnqueueWriteBuffer(queue.handle(),
                                  data_,
                                  CL_FALSE,
                                  0,
                                  size_,
                                  host,
                                  0,
                                  nullptr,
                                  &event));
    return event;
}

template<class T>
void CL::data::write(const CL::command_queue &queue, const T *host,cl_bool blocking){
    CL_ERROR(clEnqueueWriteBuffer(queue.handle(),
                                  data_,
                                  blocking,
                                  0,
                                  size_,
                                  host,
                                  0,
                                  nullptr,
                                  nullptr));
}

//Explicit Instantiation write
template CL::event CL::data::write<cl_uint>(const CL::command_queue &queue,const cl_uint *host);
template CL::event CL::data::write<cl_int>(const CL::command_queue &queue,const cl_int *host);
template CL::event CL::data::write<cl_float>(const CL::command_queue &queue,const cl_float *host);
template CL::event CL::data::write<cl_float2>(const CL::command_queue &queue,const cl_float2 *host);

template void CL::data::write<cl_uint>(const CL::command_queue &queue,const cl_uint *host,cl_bool blocking);
template void CL::data::write<cl_int>(const CL::command_queue &queue,const cl_int *host,cl_bool blocking);
template void CL::data::write<cl_float>(const CL::command_queue &queue,const cl_float *host,cl_bool blocking);
template void CL::data::write<cl_float2>(const CL::command_queue &queue,const cl_float2 *host,cl_bool blocking);

