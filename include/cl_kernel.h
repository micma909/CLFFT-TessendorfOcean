#ifndef __clLib__cl_kernel__
#define __clLib__cl_kernel__
#include <iostream>

#include "cl_program.h"
#include "cl_data.h"
#include "cl_event.h"

namespace CL{
    class kernel {
    protected:
        cl_kernel kernel_;
        std::string name_;
		template<class T>
		T work_group_info(const CL::device &d, cl_kernel_work_group_info info) const;        
    private:
        template<typename T,typename ...Ts>
        void setArgs_(int i,const T arg){
            setArg(i, arg);
        }
        template<typename T,typename ...Ts>
        void setArgs_(int i,const T arg,const Ts... args){
            setArg(i, arg);
            setArgs_(++i, args...);
        }
    public:
        kernel();
        kernel(const cl_kernel &kernel);
        kernel(const CL::kernel &kernel);
        kernel(CL::program program, const std::string &name);
        ~kernel();
        
        void operator=(const CL::kernel &k);
        
        template<typename T>
        void setArg(int i,const T &buffer);

        template<typename ...Ts>
        void setArgs(const Ts... args){
            setArgs_(0, args...);
        }

        void setLocalArg(int i,size_t size);

		cl_ulong private_mem_size(const CL::device &d) const;

        CL::event async_execute1D(const CL::command_queue &,
                                  size_t work_group_dim,
                                  size_t global_dim);
        CL::event async_execute2D(const CL::command_queue &,
                            size_t work_group_dim_x,
                            size_t work_group_dim_y,
                            size_t global_dim_size_x,
                            size_t global_dim_size_y);
        CL::event async_execute3D(const CL::command_queue &,
                                  size_t work_group_dim_x,
                                  size_t work_group_dim_y,
                                  size_t work_group_dim_z,
                                  size_t global_dim_size_x,
                                  size_t global_dim_size_y,
                                  size_t global_dim_size_z);
        
        void sync_execute1D(const CL::command_queue &,
                            size_t work_group_dim,
                            size_t global_dim);
        void sync_execute2D(const CL::command_queue &,
                            size_t work_group_dim_x,
                            size_t work_group_dim_y,
                            size_t global_dim_size_x,
                            size_t global_dim_size_y);
        void sync_execute3D(const CL::command_queue &,
                                  size_t work_group_dim_x,
                                  size_t work_group_dim_y,
                                  size_t work_group_dim_z,
                                  size_t global_dim_size_x,
                                  size_t global_dim_size_y,
                                  size_t global_dim_size_z);
        
        cl_kernel handle() const;
    };
}

#endif /* defined(__clLib__cl_kernel__) */
