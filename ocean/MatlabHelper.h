#ifndef MatlabHelper_h
#define MatlabHelper_h
#include <engine.h>
#include <mex.h>
#define BUF_SIZE (1024*2)

#define MATLAB_BIN "/Applications/MATLAB_R2013a.app/bin/matlab"


// The fixture for testing class Foo.
class MatlabHelper {
public:
    static Engine *createEngine(){
        Engine *engine = engOpen(MATLAB_BIN " -nodesktop -nosplash");
        char buffer[BUF_SIZE];
        buffer[BUF_SIZE-1] = '\0';
        engOutputBuffer(engine, buffer, BUF_SIZE);
        return engine;
    }
    
    static void copyFromMatlabComplex(mxArray *m,cl_float *o){
        const mwSize *N = mxGetDimensions(m);
        float *re = (float*)mxGetData(m);
        float *im = (float*)mxGetImagData(m);
        for (int j = 0; j < N[1]; j++) {
            for (int i = 0; i < N[0]; i++) {
                o[2*i+2*j*N[0]+0] = re[i+j*N[0]];//Real
                o[2*i+2*j*N[0]+1] = im[i+j*N[0]];//Im
            }
        }
    }
    
    static void copyToMatlabComplex(const cl_float *o,mxArray *m){
        const mwSize *N = mxGetDimensions(m);
        float *re = (float*)mxGetData(m);
        float *im = (float*)mxGetImagData(m);
        for (int j = 0; j < N[1]; j++) {
            for (int i = 0; i < N[0]; i++) {
                re[i+j*N[0]] = o[2*i+2*j*N[0]+0];//Real
                im[i+j*N[0]] = o[2*i+2*j*N[0]+1];//Im
            }
        }
    }
    
    static void copyFromMatlabReal(mxArray *m,cl_float *o){
        const mwSize *N = mxGetDimensions(m);
        float *re = (float*)mxGetData(m);
        for (int j = 0; j < N[1]; j++) {
            for (int i = 0; i < N[0]; i++) {
                o[i+j*N[0]+0] = re[i+j*N[0]];//Real
            }
        }
    }
    
    static void copyToMatlabReal(const cl_float *o,mxArray *m){
        const mwSize *N = mxGetDimensions(m);
        float *re = (float*)mxGetData(m);
        for (int j = 0; j < N[1]; j++) {
            for (int i = 0; i < N[0]; i++) {
                re[i+j*N[0]] = o[i+j*N[0]];//Real
            }
        }
    }
};


#endif
