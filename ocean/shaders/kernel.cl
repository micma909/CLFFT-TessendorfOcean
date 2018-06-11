#define DEBUG 0


int shift(int p,int n_2);
int ishift(int i,int n_2);
float cotangent(float3 a,float3 b,float3 c);
float curvature(read_only image2d_t displacement,float3 N,int n);

__constant sampler_t curvatureSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;
__constant sampler_t normalSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_REPEAT | CLK_FILTER_LINEAR;


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

__kernel void fftWave(__global float* Y,
					  __global float* X,
					  __global float* Z,
                      __global float* A,
                      __global float* theta,
                      float L,
                      int n,
                      float t){
    
    const int n_2 = (n)/2;
	int p = get_global_id(0);
	int q = get_global_id(1);
    
    int i = shift(p,n_2);
    int j = shift(q,n_2);
    
    
    if( p < n && q < n){
#if DEBUG
        if(!(n % 2)) printf("n = %d, but has to be odd!!!",n);
#endif
        //---
        const int idx_p = (int)(ishift(i,n_2)+n*ishift(j,n_2));
        const int idx_n = (int)(ishift(-i,n_2)+n*ishift(-j,n_2));
        
#if DEBUG
        if(idx_p < 0 || idx_p > (n*n-1) || idx_n < 0 || idx_n > (n*n-1)){
            printf("BAD! n:%d, n_2(n/2):%d, i:%d j:%d => idx_p %d,idx_n %d\n",n,n_2,i,j,idx_p,idx_n);
        }
#endif
        
        float k = 2.0*M_PI_F*sqrt((float)i*(float)i+(float)j*(float)j)/L;
        float w = sqrt(9.82*k);
        
		float2 k_vec = 2*M_PI_F*(float2)(i,j)/L;
        
        int real = 2*idx_p;
		int complex = 2*idx_p+1;
        
        float lamda = 1.0;
        
        if(k != 0){

			float s = .5f;
            //Real
            Y[real] =  s*cos(theta[idx_p] - w*t )*A[idx_p] + s*cos(theta[idx_n] - w*t) * A[idx_n];
            
            //Complex
            Y[complex] = s*sin(theta[idx_p] - w*t )*(A[idx_p] - A[idx_n]);
            
            X[real] = lamda*k_vec.x/k * (-1)*Y[complex];
            X[complex] = lamda*k_vec.x/k * Y[real];
            
            Z[real] = lamda*k_vec.y/k * (-1)*Y[complex];
            Z[complex] = lamda*k_vec.y/k * Y[real];
        }else{
            //Real
            Y[real] =  0;
            
            //Complex
            Y[complex] = 0;
            
            X[real] = 0;
            X[complex] = 0;
            
            Z[real] = 0;
            Z[complex] = 0;
        }
        
        
    }
    
}


__kernel void setHeight(write_only image2d_t displacement,
	                    __global float* y,
						__global float* x,
						__global float* z,
						int n,
						int simplify){
    int i = get_global_id(0);
    int j = get_global_id(1);
    
	
	float v_x, v_y, v_z;
	
	v_y = y[2*i+j*2*n];

	if(!simplify){ 
		 v_x = x[2*i+j*2*n];
		 v_z = z[2*i+j*2*n];
	}else{
		v_x = 0.f;
		v_z = 0.f;
	}

    
    
    #define OFFSET(x) ( j%2 ? ( i%2 ? x : -x ) : ( i%2 ? -x : x ) )
    write_imagef(displacement,(int2)(i,j),(float4)(OFFSET(v_x),OFFSET(v_y),OFFSET(v_z),0));
}


__kernel void setNormal(write_only image2d_t normals,
                        read_only image2d_t displacement,
                        int n){
    
    
    int i = get_global_id(0);
    int j = get_global_id(1);
    float4 p0 = (float4)(i,0,j,0)+read_imagef(displacement,normalSampler,(int2)(i,j));
    float4 p1 = (float4)(i+1,0,j,0)+read_imagef(displacement,normalSampler,(int2)(i+1,j));
    float4 p2 = (float4)(i,0,j+1,0)+read_imagef(displacement,normalSampler,(int2)(i,j+1));

    float3 v1 = normalize(p1.xyz-p0.xyz);
    float3 v2 = normalize(p2.xyz-p0.xyz);
    float3 N = cross(v2,v1);
    
    //compute curvature
    float c = curvature(displacement,N,n);
    
    //Write normal + curvature
    write_imagef(normals,(int2)(i,j),(float4)(N,c));
}

float cotangent(float3 a,float3 b,float3 c){
    float3 ba = a - b;
    float3 bc = c - b;
    float cot =  dot(bc,ba) / length(cross(bc, ba));
    return cot;
}


#define ROUND_IDX(x,y) (3*((x+n) % n) + 3 * ((y+n) % n)*n)
#define X(x,y) (ROUND_IDX(x,y))
#define Y(x,y) (ROUND_IDX(x,y)+1)
#define Z(x,y) (ROUND_IDX(x,y)+2)
#define V(x,y) ((float3)(x,0,y)+read_imagef(displacement,curvatureSampler,(int2)(x,y)).xyz)


float curvature(read_only image2d_t displacement,float3 N,int n){
   
    int i = get_global_id(0);
    int j = get_global_id(1);
    if(i > n || j > n) return 0.f;

    float3 v = V(i,j);
	int k;
	float cot_alfa[6],cot_beta[6];
    float3 v_n[6];
    v_n[0] = V(i,j-1);
    v_n[1] = V(i+1,j-1);
    v_n[2] = V(i+1,j);
    v_n[3] = V(i,j+1);
    v_n[4] = V(i-1,j+1);
    v_n[5] = V(i-1,j);

	float3 tmp[3];
    for(k = 0; k < 6; k++){
        tmp[0] = v_n[k];
        tmp[1] = v_n[((k+1) + 6) % 6];
        tmp[2] = v_n[((k-1) + 6) % 6];

        //Alfa
        cot_alfa[k] = cotangent(tmp[0],tmp[2],v);
        //Beta
        cot_beta[k] = cotangent(tmp[0],tmp[1],v);
    }
	
    //Area (Av)
    float Av = 0;
    for(k = 0; k < 6; k++){
        tmp[0] = v-v_n[k];
        Av += (cot_alfa[k]+cot_beta[k])*dot(tmp[0],tmp[0]);
    }
    Av *= 1.f/8.f;
	
    //Curvature (Hn)
    tmp[0] = 0;
    for(int k = 0; k < 6; k++){
        tmp[0] += (cot_alfa[k]+cot_beta[k])*(v-v_n[k]);
    }
    tmp[0] *= 1.f/(4.f*Av);
    
    float c = dot(tmp[0],N);
    if(isnan(c)){
        c = 0;
    }
    
    return c;
}

