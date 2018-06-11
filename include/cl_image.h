#ifndef CLWL_cl_image_h
#define CLWL_cl_image_h

#include <iostream>
#include "cl_mem.h"
#include "cl_data.h"

namespace CL{
    template<int D = 2>
    class image{
    private:
        //Dimensions
        size_t dim_[D];
        
        //Device data
        CL::data data_;
        
    public:
		image();
        image(CL::context &ctx,
                cl_mem_flags memflags,
                const cl_image_format*,
                const cl_image_desc*,
                size_t dim[D]);
        image(const CL::image<D> &t);
        ~image();
        void operator=(const CL::image<D> &k);
        CL::data data();
        
        CL::event write(CL::command_queue &queue, const void *data, cl_bool blocking);
        
    };
    
    //Factory methods
    CL::image<2> image2D_rgbaf(CL::context &ctx,size_t X,size_t Y);
    CL::image<2> image2D_rgbf(CL::context &ctx,size_t X,size_t Y);
    CL::image<3> image3D_rgbaf(CL::context &ctx,size_t X,size_t Y,size_t Z);

}


#endif
