#ifndef __clLib__cl_data_object__
#define __clLib__cl_data_object__

#include <iostream>

#include "cl_context.h"
#include "cl_command_queue.h"
#include "cl_helper.h"
#include "cl_event.h"
#include "cl_mem.h"

namespace CL{
    class data : public CL::mem{
    private:
        template<class T>
        T info(cl_mem_info info) const;
    protected:
        cl_mem data_;
        size_t size_;
    public:
        data();
        data(const cl_mem &data,size_t size);
        data(const CL::data &data);

        data(const CL::context &context);
        data(const CL::context &context,cl_mem_flags flags,size_t bytes);
        ~data();

        
        void operator=(const CL::data &k);

        size_t size() const;

        virtual cl_mem handle() const override;
        virtual cl_mem& handle() override;
        
        size_t mem_size() const;

        template<class T>
        CL::event read(const CL::command_queue &queue, T *host) const;
        template<class T>
        void read(const CL::command_queue &queue, T *host,cl_bool blocking) const;
        
        template<class T>
        CL::event write(const CL::command_queue &queue, const T *host);
        template<class T>
        void write(const CL::command_queue &queue, const T *host,cl_bool blocking);
    };
}


#endif /* defined(__clLib__cl_data_object__) */
