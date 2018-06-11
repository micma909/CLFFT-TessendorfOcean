#ifndef __clLib__cl_mem_object__
#define __clLib__cl_mem_object__

#include <iostream>
#include "cl_external_libraries.h"
#include "cl_helper.h"

namespace CL{
    class mem {
    public:
        mem(){}
        virtual ~mem(){}
        cl_uint referenceCount() const{
            cl_uint count;
            CL_ERROR(clGetMemObjectInfo(handle(),
                                        CL_MEM_REFERENCE_COUNT,
                                        sizeof(cl_uint),
                                        &count,
                                        nullptr));
            return count;
        }
        virtual cl_mem handle() const = 0;
        virtual cl_mem& handle() = 0;
    };
}


#endif /* defined(__clLib__cl_mem_object__) */
