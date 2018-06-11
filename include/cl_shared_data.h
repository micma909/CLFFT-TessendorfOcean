#ifndef CLWL_cl_array_h
#define CLWL_cl_array_h

#include <iostream>
#include "cl_mem.h"
#include "cl_data.h"

namespace CL{
    template<typename T>
    class shared_data : public CL::mem{
    private:
        //Host data
        T *host_data;
        long int N;
        
        //Device data
        CL::data data;

    public:
		shared_data();
        shared_data(CL::context &ctx, cl_mem_flags memflags, long int elements);
        shared_data(const CL::shared_data<T> &t);
        ~shared_data();

        void operator=(const CL::shared_data<T> &k);

        
        T* ptr();
        long int elements() const;
        
        //Operator
        T& operator()(int i);
        T operator()(int i) const;
        T& operator[](int i);
        T operator[](int i) const;
        
        //Device to host communication
        CL::event async_read(const CL::command_queue &queue);
        void sync_read(const CL::command_queue &queue);
        
        CL::event async_write(const CL::command_queue &queue);
        void sync_write(const CL::command_queue &queue);
        
        
        CL::data dataHandle(){
            return data;
        }
        
        //CL::mem interface
        virtual cl_mem handle() const override;
        virtual cl_mem& handle() override;
    };
    
}


#endif
