#define DEBUG 0

__kernel void fftWave(__global float* Y,
                      __global float* A,
                      __global float* theta,
                      float L,
                      int n,
                      float t){
    
    const int offset = n/2-1;
	int i = get_global_id(0)-offset+1;
	int j = get_global_id(1)-offset+1;

    if( get_global_id(0) < n && get_global_id(1) < n){
        #if DEBUG
        if(!(n % 2)) printf("n = %d, but has to be odd!!!",n);
        #endif
        //---
        const int idx_p = (i+offset+(j+offset)*n);
        const int idx_n = (-i+offset+(-j+offset)*n);
        
        #if DEBUG
        if(idx_p < 0 || idx_p > (n*n-1) || idx_n < 0 || idx_n > (n*n-1)){
            printf("BAD! n:%d, offset(n/2):%d, i:%d j:%d => idx_p %d,idx_n %d\n",n,offset,i,j,idx_p,idx_n);
        }
        #endif
        
        float k = 2.0*M_PI_F*sqrt((float)i*(float)i+(float)j*(float)j)/L;
        float w = sqrt(9.82*k);
        
        //Real
        
        Y[2*idx_p+0] = 0.5*cos(theta[idx_p] - w*t )*A[idx_p] + 0.5*cos(theta[idx_n] - w*t) * A[idx_n];
        
        //Complex
        Y[2*idx_p+1] = 0.5*sin(theta[idx_p] - w*t )*A[idx_p] - 0.5*sin(theta[idx_n] - w*t) * A[idx_n];
        
    }
    
}

__kernel void setHeight(__global float* gl_verts, __global float* y){
		int i = get_global_id(0);
		int j = get_global_id(1);
		int k = get_global_size(0);
		
		float v = y[2*i+j*2*k];

		gl_verts[i*3 + 3*j*k + 1] = j%2 ? ( i%2 ? v : -v ) : ( i%2 ? -v : v );
		
		if(i >= k-1){ 
			gl_verts[i*3 + 3*j*k + 1] = gl_verts[3*j*k + 1];
		}
}

__kernel void naiveWave(__global float* gl_verts,
						 __global float* A,
						 __global float* theta, 
						 float L, 
						 int n,
						 float t){

	int p = get_global_id(0);
	int q = get_global_id(1);
	if(p < n && q < n){
		float h = 0,w;
		int i, j;
        const int offset = n/2;
        float2 k, x;
        x = (float2)((float)p*L/(float)n, (float)q*L/(float)n);
        float k_scale = 2*M_PI_F/L;
        
        for(j = -offset+1; j < offset-1; j++){
            for(i = -offset+1; i < offset-1; i++){
                int indx = i+offset + (j+offset)*n;
                k = (float2)(k_scale*(float)i, k_scale*(float)j);
                w = sqrt(length(k)*9.82);
                h += A[indx]*cos(dot(k, x) - w*t + theta[indx] );
            }
        }

		gl_verts[p*3 + 3*q*n + 1] = h;
	}
}
/*
__kernel void setHeight(__global float* gl_verts, float t){
    
	int id = get_global_id(0);
	gl_verts[id*3+1] = sin(gl_verts[id*3]*10+t)*0.2f
    + sin(gl_verts[id*3]*20+t)*0.02f
    + sin(gl_verts[id*3+2]*20+t)*0.02f;
    
}
*/
__kernel void setNormal(__global float* gl_normal_lines,__global float* gl_normals,__global float* gl_verts,float t){
	int x = get_global_id(0);
    int z = get_global_id(1);
    int Nx = get_global_size(0);
    int Nz = get_global_size(1);

    float3 normal;
    float a;
    //Either 1 or -1
    int offset = x < Nx-1 && z < Nz-1;
    offset = 2*offset - 1;
    
    float3 p1 = {gl_verts[3*x+(3*Nx)*z],
                 gl_verts[3*x+(3*Nx)*z+1],
                 gl_verts[3*x+(3*Nx)*z+2]};
    
    float3 p2 = {gl_verts[3*(x+offset)+(3*Nx)*z],
                 gl_verts[3*(x+offset)+(3*Nx)*z+1],
                 gl_verts[3*(x+offset)+(3*Nx)*z+2]};
    
    float3 p3 = {gl_verts[3*x+(3*Nx)*(z+offset)],
                 gl_verts[3*x+(3*Nx)*(z+offset)+1],
                 gl_verts[3*x+(3*Nx)*(z+offset)+2]};
    
    float3 nx = p3-p1;
    float3 nz = p2-p1;
    normal = normalize(cross(nx,nz));
    gl_normals[3*x+3*Nx*z]   = normal.x;
    gl_normals[3*x+3*Nx*z+1] = normal.y;
    gl_normals[3*x+3*Nx*z+2] = normal.z;
    
    //for drawing
    normal = normal*0.05f;
    gl_normal_lines[2*3*x+2*3*Nx*z+0]   = p1.x;
    gl_normal_lines[2*3*x+2*3*Nx*z+1]   = p1.y;
    gl_normal_lines[2*3*x+2*3*Nx*z+2]   = p1.z;
    gl_normal_lines[2*3*x+2*3*Nx*z+3]   = p1.x+normal.x;
    gl_normal_lines[2*3*x+2*3*Nx*z+4]   = p1.y+normal.y;
    gl_normal_lines[2*3*x+2*3*Nx*z+5]   = p1.z+normal.z;
    
}