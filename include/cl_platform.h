#ifndef __clUtilLib__File__
#define __clUtilLib__File__

#include <iostream>
#include <vector>
#include "cl_external_libraries.h"

#include "cl_object.h"

namespace CL{
  
    /**
     @brief
     * Manager cl_platform object.
     */
    class platform : public object<cl_platform_id>{
    public:
        platform();
        platform(cl_platform_id platform);
        ~platform();
        
        void operator=(const CL::platform &p);
        /**
            @brief Looks for all installed platforms, it will return the first platform encountered. The return value is not deterministic.
            @return platform  
         */
        static platform any_platform();
        /**
         @brief Looks for <b>plafrom_name</b> among all installed platforms unless a platform with identical name is found, it will return any of the platforms encountered.
         @return platform
         */
        static platform preferred_platform(const std::string &platform_name);
        /**
         @brief Returns an array of all available platforms
         @return std::vector<platform>
         */
        static std::vector<platform> all_platforms();
        
        //Platform info
        /**
         @brief Returns the value of CL_PLATFORM_NAME
         @return std::string
         */
        std::string name() const;
        
        /**
         @brief Prints some information about the platform. Prints the value of 
         CL_PLATFORM_PROFILE,
         CL_PLATFORM_VERSION,
         CL_PLATFORM_NAME,
         CL_PLATFORM_VENDOR,
         CL_PLATFORM_EXTENSIONS
         */
        void info(std::ostream &s) const;

        /**
         @brief Returns the cl_platform_id object managed by the class
         */
        /**
         @brief Returns the number of available platforms
         */
        static cl_uint available_platforms();
    };
    
};

#endif /* defined(__clUtilLib__File__) */
