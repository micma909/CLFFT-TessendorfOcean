#include "cl_helper.h"
#include <map>
#include "cl_exception.h"


void CL::helper::_errorHandler(cl_int ciErrNum,const char file[],const char command[],int line_nr){
    static int call_nr = 0;
    call_nr++;
    if(ciErrNum != CL_SUCCESS){
        throw CL::exception(ciErrNum,file,command,line_nr);
    }
    
    

}



