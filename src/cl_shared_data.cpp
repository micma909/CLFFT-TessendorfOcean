#include "cl_shared_data.h"

template<typename T>
CL::shared_data<T>::shared_data():N(0){
    host_data = nullptr;
}

template<typename T>
CL::shared_data<T>::shared_data(CL::context &ctx, cl_mem_flags mem_flags, long int elements):N(elements),data(ctx,mem_flags,N*sizeof(T)){
    host_data = new T[N];
}

template <typename T>
CL::shared_data<T>::shared_data(const CL::shared_data<T> &t) {
    if(host_data) delete [] host_data;
    
    N = t.N;
    host_data = new T[N];
    data = t.data;
};

template <typename T>
void CL::shared_data<T>::operator=(const CL::shared_data<T> &t){
    if(host_data) delete [] host_data;

    N = t.N;
    host_data = new T[N];
    data = t.data;
}

template<typename T>
CL::shared_data<T>::~shared_data(){
    if (host_data) delete [] host_data;
}

template<typename T>
T& CL::shared_data<T>::operator()(int i){
    return host_data[i];
}

template<typename T>
T CL::shared_data<T>::operator()(int i) const{
    return host_data[i];
}

template<typename T>
T& CL::shared_data<T>::operator[](int i){
    return host_data[i];
}

template<typename T>
T CL::shared_data<T>::operator[](int i) const{
    return host_data[i];
}

template<typename T>
CL::event CL::shared_data<T>::async_read(const CL::command_queue &queue){
    return data.read(queue, host_data);
}
template<typename T>
void CL::shared_data<T>::sync_read(const CL::command_queue &queue){
    data.read(queue, host_data, CL_TRUE);
}

template<typename T>
CL::event CL::shared_data<T>::async_write(const CL::command_queue &queue){
    return data.write(queue, host_data);
}
template<typename T>
void CL::shared_data<T>::sync_write(const CL::command_queue &queue){
    data.write(queue, host_data, CL_TRUE);
}

template<typename T>
T* CL::shared_data<T>::ptr(){
    return host_data;
}

template<typename T>
long int CL::shared_data<T>::elements() const{
    return N;
}

template<typename T>
cl_mem CL::shared_data<T>::handle() const{
    return data.handle();
}

template<typename T>
cl_mem& CL::shared_data<T>::handle(){
    return data.handle();
}

template class CL::shared_data<cl_float>;
