#ifndef __clUtilLib__cl_program__
#define __clUtilLib__cl_program__

#include <iostream>
#include <vector>

#include "cl_device.h"
#include "cl_context.h"
#include "cl_command_queue.h"

namespace CL{
    class program {
    protected:
        cl_program program_;

        static std::string source(const std::string &src_file);

        std::string build_log(const CL::device &p) const;
        
        void build(const CL::context &context,
                   const CL::device &device,
                   const std::string &source,
                   const std::string &options);
        void build(const CL::context &context,
                   const std::vector<CL::device> &devices,
                   const std::string &source,
                   const std::string &options);
        void build_for_devices(const cl_context &context,
                               const std::vector<cl_device_id> &devices,
                               const std::string &source,
                               const std::string &options);
    public:
        /**
            @brief Constructor
        */
        program();
        program(const cl_program &program);
        program(const CL::program &program);
        
        static CL::program file(CL::context &context,
                                CL::device &device,
                                const std::string &source_file,
                                const std::string &options = "");
        
        static CL::program source(CL::context &context,
                                  CL::device &device,
                                  const std::string &source,
                                  const std::string &options = "");
        ~program();
        
        void operator=(const CL::program &p);
        
        cl_program handle() const;
    };
}

#endif /* defined(__clUtilLib__cl_program__) */
