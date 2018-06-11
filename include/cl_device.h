#ifndef clUtilLib_cl_device_h
#define clUtilLib_cl_device_h

#include "cl_platform.h"
namespace CL{
    
    /**
     @brief Manager of opencl device
     */
    class device{
    private:
        //Info
        template<class T>
        T info(cl_device_info) const;
    protected:
        cl_device_id device_id_;
    public:
        /**
         @brief Constructors
         */
        device();
        device(const cl_device_id &device_id);
        device(const CL::device &device);
        ~device();
        
        void operator=(const CL::device &p);

        /**
         @brief Constructors
         */
        static device preferred_device(const CL::platform &platform, const std::string &device_name);
        static device preferred_device(const CL::platform &platform, cl_device_type device_type);
        static device any_gpu_device(const CL::platform &platform);
		static device default_device(const CL::platform &platform);
		static device any_avaiable_device(const CL::platform &platform);
        static device any_avaiable_gpu(const CL::platform &platform);
        static device manually(const CL::platform &platform);
        static device supporting_extension(const CL::platform &platform, const std::vector<std::string> &extensions, cl_device_type device_type);
        static void print_all_devices(const CL::platform &platform,std::ostream &stream = std::cout);

        static std::vector<device> all_devices(const CL::platform &platform,  cl_device_type device_type);
        static std::vector<device> all_available_devices(const CL::platform &platform,  cl_device_type device_type);

        /**
         @brief Getters
         */
        cl_device_id handle() const;
        /**
         @brief Info
         */
        std::string name() const;
        bool available() const;
        cl_uint max_clock_frequency() const;
        cl_ulong global_mem_size() const;
        cl_uint max_compute_units() const;
        cl_uint vendor_id() const;

        std::string extensions() const;
		bool supports_extension(const std::string& extension) const;

        cl_uint reference_count() const;


        /**
         @brief Helpers
         */
		static cl_uint available_devices(const CL::platform& platform,
                                        cl_device_type device_type);
        
        void info(std::ostream &s) const;

    };
    
};


#endif
