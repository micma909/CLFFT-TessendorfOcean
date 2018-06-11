#ifndef __clLib__cl_object__
#define __clLib__cl_object__

#include <iostream>

namespace CL{
    template<class T>
    class object {
    protected:
        T object_;
    public:
        virtual ~object(){}

        void operator=(const CL::object<T> &o){ object_ = o; }
        
        T handle() const{ return object_; }
        T& handle(){ return object_; }
    };
}
#endif /* defined(__clLib__cl_object__) */
