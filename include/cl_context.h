#ifndef __clUtilLib__cl_context__
#define __clUtilLib__cl_context__

#include <iostream>
#include <vector>
#include "cl_device.h"

namespace CL{
    /**
     OpenCL callback struct
     */
    class context {
    private:
        /**
         @brief Called from contructor
         */
        void init(const std::vector<cl_device_id> &devices);
    protected:
        cl_context context_;
        std::ostream *stream_ ;
        
		static void CL_CALLBACK context_callback(const char *, const void *, size_t, void *);
        

    public:
        /**
         @brief Constructors
         */
        context();
        context(const cl_context &context);
        context(const CL::context &context);
        context(const CL::device &device,std::ostream *stream = &std::cout);
        context(const std::vector<CL::device> &devices,std::ostream *stream = &std::cout);
        ~context();
        
        // Get a list of the devices associated with the context
        std::vector<CL::device> getDevices() const;
        
        void operator=(const CL::context &c);

        /**
         @brief info
         */
        cl_uint reference_count() const;
        /**
         @brief
         */
        cl_context handle() const;
    };
}
#endif /* defined(__clUtilLib__cl_context__) */
