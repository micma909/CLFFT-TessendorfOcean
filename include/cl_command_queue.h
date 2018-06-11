#ifndef __clLib__cl_command_queue__
#define __clLib__cl_command_queue__

#include <iostream>


#include "cl_context.h"
#include "cl_device.h"

namespace CL{
    class command_queue {
    protected:
        cl_command_queue command_queue_;

    public:
        command_queue();
        command_queue(const cl_command_queue cq);
        command_queue(const CL::command_queue &cq);
        command_queue(const context &context, const device &device, bool profilingEnabled = false);
        ~command_queue();
        
        void operator=(const CL::command_queue &c);

        void wait() const;
        /**
         @brief
         */
        cl_command_queue handle() const;
        cl_command_queue& handle();
    };
}
#endif /* defined(__clLib__cl_command_queue__) */
