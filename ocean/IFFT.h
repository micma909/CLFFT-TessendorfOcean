#ifndef CLWL_IFFT_h
#define CLWL_IFFT_h

#include <cl_lib.h>
#include <clFFT.h>

#include <iostream>

#define clFFT_ERROR(error) _error(error,__FILE__,#error,__LINE__)


class FFT2{
private:
    void _error(clfftStatus status,const char *file,const char *function,int line){
        //clfftStatus is superset of opencl errors
 
        

        
        try {
            CL_ERROR_CUSTOM(status,file,function,line);
        } catch (CL::exception &e) {
#define CHECK(E_NAME) case E_NAME: ss << #E_NAME << "( code = " << status << " )" << std::endl; e.addAdditionalInfo(ss.str(),"CLFFT");  break
            if (!e.is_known()) {
                std::stringstream ss;
                switch (status) {
                    case CLFFT_SUCCESS:
                        break;
                    CHECK(CLFFT_NOTIMPLEMENTED);
                    CHECK(CLFFT_TRANSPOSED_NOTIMPLEMENTED);
                    CHECK(CLFFT_FILE_NOT_FOUND);
                    CHECK(CLFFT_FILE_CREATE_FAILURE);
                    CHECK(CLFFT_VERSION_MISMATCH);
                    CHECK(CLFFT_INVALID_PLAN);
                    CHECK(CLFFT_DEVICE_NO_DOUBLE);
                    CHECK(CLFFT_DEVICE_MISMATCH);
                    default:
                        ss << "UNKNOWN_ERROR( code = " << status << " )" << std::endl;
                        e.addAdditionalInfo(ss.str(),"CLFFT");
                    break;
                }
                throw e;
            }
        }
    
    }
protected:
    clfftPlanHandle *planHandle;
    clfftSetupData *fftSetup;
    CL::command_queue queue_;
    size_t size[2];
	CL::context _ctx;
private:
    void operator=(const FFT2&) = delete;
    void operator=(const FFT2&&) = delete;
    void operator=(const FFT2*) = delete;
public:
    FFT2(){}
    void init( const CL::context &ctx,
              const CL::command_queue &queue,size_t Nx,size_t Ny ){
        queue_ = queue;
        size[0] = Nx;
        size[1] = Ny;
        _ctx = ctx;

        /* Setup clFFT. */
        fftSetup = new clfftSetupData();
        clFFT_ERROR(clfftInitSetupData(fftSetup));
        clFFT_ERROR(clfftSetup(fftSetup));
        cl_context c = ctx.handle();
        /* Create a default plan for a complex FFT. */

        planHandle = new clfftPlanHandle();
        clFFT_ERROR(clfftCreateDefaultPlan(planHandle,
                                           c,
                                           CLFFT_2D,
                                           size));
        
        /* Set plan parameters. */
        clFFT_ERROR(clfftSetPlanPrecision(*planHandle, CLFFT_SINGLE));
		clFFT_ERROR(clfftSetLayout(*planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED));
        clFFT_ERROR(clfftSetResultLocation(*planHandle, CLFFT_OUTOFPLACE));
        clFFT_ERROR(clfftSetPlanTransposeResult(*planHandle,CLFFT_NOTRANSPOSE));
        
        size_t in_stride[2] = {1,Nx};
        clFFT_ERROR(clfftSetPlanInStride(*planHandle,CLFFT_2D,in_stride));
        
        size_t out_stride[2] = {1,Nx};
        clFFT_ERROR(clfftSetPlanOutStride(*planHandle,CLFFT_2D,out_stride));
        
        /* Bake the plan. */
        cl_command_queue q = queue_.handle();
        clFFT_ERROR(clfftBakePlan(*planHandle, 1,&q , NULL, NULL));
    }
    
    void destroy(){
        /* Release the plan. */
        if(planHandle != nullptr) clFFT_ERROR(clfftDestroyPlan(planHandle));
    }
    
    static void teardown(){
        clfftTeardown();
    }
    
    ~FFT2(){

        if(fftSetup != nullptr) delete fftSetup;
        if(planHandle != nullptr) delete planHandle;

        /* Release clFFT library. */
        // clFFT_ERROR(clfftTeardown());
    }

    
    void operator()(CL::data in_mem,CL::data out_mem){
        ifft(in_mem, out_mem);
    }
    
    void _fft(CL::data in_mem,CL::data out_mem,clfftDirection dir){
        cl_command_queue q = queue_.handle();
        cl_mem in_m  = in_mem.handle();
        cl_mem out_m = out_mem.handle();

		cl_event e;
		// Execute the plan.
		clFFT_ERROR(clfftEnqueueTransform(*planHandle,
										  dir,
										  1,
										  &q,
										  0,
										  NULL,
										  &e,
										  &in_m,
										  &out_m,
										  NULL));
		CL_ERROR(clWaitForEvents(1, &e));
		clFinish(q);
        
    }
    
    static void ishift(cl_float *ptr,int Nx,int Ny){
        for (int j = 0; j < Nx/2; j++) {
            for (int i = 0; i < Ny/2; i++) {
                //Swap first and fourth
                std::swap(ptr[2*i+2*j*Nx], ptr[2*(i+Nx/2)+2*(j+Nx/2)*Nx]);
                std::swap(ptr[2*i+2*j*Nx+1], ptr[2*(i+Nx/2)+2*(j+Nx/2)*Nx+1]);
                
                //Swap second and third
                std::swap(ptr[2*(i+Nx/2)+2*j*Nx], ptr[2*(i)+2*(j+Nx/2)*Nx]);
                std::swap(ptr[2*(i+Nx/2)+2*j*Nx+1], ptr[2*(i)+2*(j+Nx/2)*Nx+1]);
                
            }
        }
    }
    
    void ifft(CL::data in_mem,CL::data out_mem){
        // assert(in_mem.size() == 2*sizeof(cl_float)*size[0]*size[1]);
        // assert(out_mem.size() == 2*sizeof(cl_float)*size[0]*size[1]);
        _fft(in_mem, out_mem, CLFFT_BACKWARD);
    }
    
    void fft(CL::data in_mem,CL::data out_mem){
        // assert(in_mem.size() == 2*sizeof(cl_float)*size[0]*size[1]);
        // assert(out_mem.size() == 2*sizeof(cl_float)*size[0]*size[1]);
        _fft(in_mem, out_mem, CLFFT_FORWARD);
    }
    
};



#endif
