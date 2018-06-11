#ifndef clUtilLib_cl_helper_h
#define clUtilLib_cl_helper_h

#define CL_ERROR_CUSTOM(function,FILE,FUNCTION,LINE) CL::helper::_errorHandler(function,FILE,FUNCTION,LINE)
#define CL_ERROR(function)                           CL_ERROR_CUSTOM(function,__FILE__,#function,__LINE__)

#include "cl_external_libraries.h"

namespace CL{
    namespace helper{
        //Error handler
        //void ERROR(cl_int error);
        void _errorHandler(cl_int ciErrNum,const char file[],const char command[],int line_nr);

    }
};

#endif
