#include "Spectrum.h"
#include <gl/glew.h>
#include <cl_lib.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>
#include <assert.h>

//Aij
/*
void Spectrum::phillips_wave(int n,float L,const float wind[2],cl_float *spectrum){
    std::default_random_engine generator;
    generator.seed(120120);
    std::normal_distribution<float> distribution;
    //std::cout << "mean:" << distribution.mean() << " standard deviation:" << distribution.stddev() << std::endl;
    
    
    auto freq_nr = [&](int k[2])->float{
        return 2*M_PI*std::sqrt((float)k[0]*(float)k[0]+(float)k[1]*(float)k[1])/L;
    };
    auto dot = [](const float a[2],const float b[2])->float{
        return a[0]*b[0]+a[1]*b[1];
    };

    float V = std::sqrt(dot(wind,wind));
    float wind_dir[2] = {wind[0]/V,wind[1]/V};
    int n_2 = n/2;
    for (int j  = -n_2 ; j < n_2; j++) {
        for (int i = -n_2; i < n_2; i++) {
            if (i == 0 || j == 0 || i == n_2-1 || j == n_2-1) {
                spectrum[(i+n_2)+(j+n_2)*n] = 0;
            }else{
                float A = 30.f;
                float k_vec[2];
                k_vec[0] = 2.f*(float)M_PI*(float)i/L;
                k_vec[1] = 2.f*(float)M_PI*(float)j/L;
                
                int idx[2];
                idx[0] = i;
                idx[1] = j;
                float k = freq_nr(idx);
                
                float k_length = std::sqrt(dot(k_vec,k_vec));
                float k_vec_norm[2];
                k_vec_norm[0] = k_vec[0]/k_length;
                k_vec_norm[1] = k_vec[1]/k_length;

                float abs = std::pow(dot(k_vec_norm,wind_dir),2.f);
                float thing = std::exp(-1.f/std::pow(k*(V*V/9.82),2))/(k*k*k*k);
                spectrum[(i+n_2)+(j+n_2)*n] = A*thing*abs;
                
                int min[] = {1,1};
                if (k < freq_nr(min)) {
                    spectrum[(i+n_2)+(j+n_2)*n] = 0;// A*thing*abs;//*distribution(generator);
                }
            }
        }
    }
}*/
/*
void Spectrum::phillips_wave(int n,float L,const float wind[2],cl_float *spectrum){
   
    auto freq_nr = [&](int k[2])->float{
        return 2*M_PI*std::sqrt((float)k[0]*(float)k[0]+(float)k[1]*(float)k[1])/L;
    };
    auto dot = [](const float a[2],const float b[2])->float{
        return a[0]*b[0]+a[1]*b[1];
    };
    
    float V = std::sqrt(dot(wind,wind));
    float wind_dir[2] = {wind[0]/V,wind[1]/V};
    int n_2 = n/2;
    for (int j  = -n_2 ; j < n_2; j++) {
        for (int i = -n_2; i < n_2; i++) {
            float A = 0.1f;
            float k_vec[2];
            k_vec[0] = 2.f*(float)M_PI*(float)i/L;
            k_vec[1] = 2.f*(float)M_PI*(float)j/L;
            float k_length = std::sqrt(dot(k_vec,k_vec));
            float k_vec_unit[2];
            k_vec_unit[0] = k_vec[0]/k_length;
            k_vec_unit[1] = k_vec[1]/k_length;
            
            int idx[2];
            idx[0] = i;
            idx[1] = j;
            float k = freq_nr(idx);
            if(k < 0.0000001){
                spectrum[(i+n_2)+(j+n_2)*n] = 0;
                continue;
            }
            
            
            float k_length2 = k_length*k_length;
            float k_length4 = k_length2*k_length2;
            
            float k_dot_w = dot(k_vec_unit,wind_dir);
            float k_dot_w2 = k_dot_w*k_dot_w*k_dot_w*k_dot_w*k_dot_w*k_dot_w;
            
            float L = V*V/9.82;
            float L2 = L*L;
  
            float damping = 0.001;
            float l2 = L2*damping*damping;
            
            spectrum[(i+n_2)+(j+n_2)*n] = A*std::exp(-1.0/(k_length2*L2))/k_length4*k_dot_w2*std::exp(-k_length2*l2);
            
        }
    }
}*/



int shift(int p,int n_2){
    if(p >= n_2){
        return p-n_2+1;
    }else{
        return p-n_2;
    }
}

int ishift(int i,int n_2){
    if(i > 0){
        return i+n_2-1;
    }else{
        return i+n_2;
    }
}

void Spectrum::phillips_wave(int n,float Lx,const float wind_vec[2],cl_float *spectrum){
    auto freq_nr = [&](int k[2])->float{
        return 2*M_PI*std::sqrt((float)k[0]*(float)k[0]+(float)k[1]*(float)k[1])/Lx;
    };
    auto dot = [](const float a[2],const float b[2])->float{
        return a[0]*b[0]+a[1]*b[1];
    };
    
    auto normalize = [](float *a)->void{
        float length = std::sqrt(a[0]*a[0]+a[1]*a[1]);
        a[0] /= length;
        a[1] /= length;
    };
    
    auto clamp = [](float v,float min,float max)->float{
        
        if (v > max) {
            return max;
        }else if (v < min){
            return min;
        }else{
            return v;
        }
    };
    
    float windspeed = std::sqrt(dot(wind_vec,wind_vec));
    float wind_dir[2] = {wind_vec[0]/windspeed,wind_vec[1]/windspeed};
    float L = windspeed * windspeed / 9.82;
    int min[2] = {0,0};
    float mink = freq_nr(min);
    float dirmove = 1.0f;
    float dirbias = 2.f;

    float scale = 100000.f;//64
    switch (n) {
        case 2048:
            scale = 8000000000000.f;
        break;
        case 1024:
            scale = 200000000000;
            break;
        case 512:
            scale = 5000000000;
            break;
        case 256:
            scale = 90000000;
            break;
        case 128:
            scale = 5000000;
            break;
        case 64:
            scale = 100000.f;
            break;
        case 32:
            scale = 1000.f;
            break;
        default:
            break;
    }
    
    
    int n_2 = n/2;
    for (int j  = -n_2,q = 0; j <= n_2; j++) {
        if (j == 0) continue;
        int counter = 0;
        for (int i = -n_2,p = 0; i <= n_2; i++) {
            if (i == 0) continue;
            int index = ishift(i, n_2) + ishift(j, n_2)*n;
            
            counter++;
            assert(counter <= n);
            
            assert(index == (p+q*n));
            p++;
            float wave_vector[2];
            wave_vector[0] = 2.f*(float)M_PI*(float)i/Lx;
            wave_vector[1] = 2.f*(float)M_PI*(float)j/Lx;
            normalize(wave_vector);
            int idx[2];
            idx[0] = i;
            idx[1] = j;

            float k = freq_nr(idx);
            if (k < 0.000001) {
                spectrum[index] = 0;
            }else{
                float k2 = k*k;
                float k2L2 = k2*L*L;
                float k2l2 = k2*mink*mink;
                float k4 = k2 * k2;
                float P = scale * std::exp(-1.f / k2L2 - k2l2) / k4;
                // reduce by wind movement
                float dotprod = dot(wave_vector, wind_dir);
                if (dotprod < 0)
                    dotprod *= clamp((1 - dirmove), 0.00, 1);
                if (dirbias > 0)
                    P *= std::pow(std::abs(dotprod), dirbias);
                spectrum[index] = std::sqrt(P);
            }
        }
        q++;
    }
}

void Spectrum::random_phase(int n,cl_float *phase){
   // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (3216531531);
    std::uniform_real_distribution<float> distribution(0,2*M_PI);

    
    for (int j  = 0 ; j < n; j++) {
        for (int i = 0; i < n; i++) {
            phase[i+n*j] = distribution(generator);
        }
    }
}