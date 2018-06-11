#include <iostream>
#include <algorithm>
#include "cl_image.h"
template<int D>
CL::image<D>::image():data_(){
    std::fill(dim_, dim_+D, 0);
}

template<int D>
CL::image<D>::image(CL::context &ctx,
                     cl_mem_flags memflags,
                     const cl_image_format*image_format,
                     const cl_image_desc*image_desc,
                     size_t dim[D]){
    cl_int error;
    cl_mem mem = clCreateImage(ctx.handle(),
                               memflags,
                               image_format,
                               image_desc,
                               NULL,
                               &error);
    CL_ERROR(error);
    
    //Copy dimensions
    std::copy(dim, dim+D, dim_);
    size_t elements = 1;
    std::for_each(dim_, dim_+D, [&](size_t d){
        elements *= d;
    });
    
    data_ = CL::data(mem,elements);
}
template<int D>
CL::image<D>::image(const CL::image<D> &t):data_(t.data_){
    std::copy(t.dim_, t.dim_+D, dim_);
}
template<int D>
CL::image<D>::~image(){
    
}
template<int D>
void CL::image<D>::operator=(const CL::image<D> &k){
    std::copy(k.dim_, k.dim_+D, dim_);
    data_ = k.data_;
}
template<int D>
CL::data CL::image<D>::data(){
    return data_;
}

template <int D>
CL::event CL::image<D>::write(CL::command_queue &queue, const void *data, cl_bool blocking) {
    
    size_t origin[3] = {0,0,0};
    size_t region[3] = {1,1,1};
    for (int i = 0; i < D; ++i) {
        region[i] = dim_[i];
    }
    
    cl_event event;
    CL_ERROR(clEnqueueWriteImage(queue.handle(),
                        data_.handle(),
                        blocking,
                        origin,
                        region,
                        dim_[0],
                        0,
                        data,
                        0,
                        nullptr,
                        &event));
    
    return CL::event(event);
}

//Explicit instantiation
template class CL::image<1>;
template class CL::image<2>;
template class CL::image<3>;


CL::image<2> CL::image2D_rgbaf(CL::context &ctx,size_t X,size_t Y){
    cl_image_format format = {CL_RGBA,CL_FLOAT};
    cl_image_desc descr = { CL_MEM_OBJECT_IMAGE2D,  //image_type
        X,                      //image_width
        Y,                      //image_height
        NULL,                   //image_depth
        NULL,                   //image_array_size
        NULL,                   //image_row_pitch
        NULL,                   //image_slice_pitch
        0,                      //num_mip_levels
        0,                      //num_samples
        NULL};                  //buffer
    
    
    size_t dim[2] = {X,Y};
    return CL::image<2>(ctx,
                        CL_MEM_READ_WRITE,
                        &format,
                        &descr,
                        dim);
}

CL::image<2> CL::image2D_rgbf(CL::context &ctx,size_t X,size_t Y){
    cl_image_format format = {CL_RGB,CL_FLOAT};
    cl_image_desc descr = { CL_MEM_OBJECT_IMAGE2D,  //image_type
        X,                      //image_width
        Y,                      //image_height
        NULL,                   //image_depth
        NULL,                   //image_array_size
        NULL,                   //image_row_pitch
        NULL,                   //image_slice_pitch
        0,                      //num_mip_levels
        0,                      //num_samples
        NULL};                  //buffer
    
    
    size_t dim[2] = {X,Y};
    return CL::image<2>(ctx,
                        CL_MEM_READ_WRITE,
                        &format,
                        &descr,
                        dim);
}

CL::image<3> CL::image3D_rgbaf(CL::context &ctx,size_t X,size_t Y,size_t Z){
    cl_image_format format = {CL_RGBA,CL_FLOAT};
    cl_image_desc descr = { CL_MEM_OBJECT_IMAGE3D,  //image_type
        X,                      //image_width
        Y,                      //image_height
        Z,                      //image_depth
        NULL,                   //image_array_size
        NULL,                   //image_row_pitch
        NULL,                   //image_slice_pitch
        0,                      //num_mip_levels
        0,                      //num_samples
        NULL};                  //buffer
    
    
    size_t dim[3] = {X,Y,Z};
    return CL::image<3>(ctx,
                        CL_MEM_READ_WRITE,
                        &format,
                        &descr,
                        dim);
}







