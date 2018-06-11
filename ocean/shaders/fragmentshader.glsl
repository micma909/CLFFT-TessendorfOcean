#version 330 

in vec3 normal_vector;
in vec3 light_vector;
in vec3 halfway_vector;
in float fog_factor;
in vec3 original_vertex;

uniform int g;
uniform sampler2D normalMap;

out vec4 fragColor;
uniform float time;

//--------

in vec3 vTexCoord3DSmall;
in vec3 vTexCoord3DLarge;
in vec3 v;

vec4 permute(vec4 x)
{
  return mod(((x*34.0)+1.0)*x, 289.0);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod(i, 289.0 ); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }
//--------
/*
void main( void )
{
  float n = snoise(vTexCoord3D);

  gl_FragColor = vec4(0.5 + 0.6 * vec3(n, n, n), 1.0);
}
*/
void main (void){


    fragColor  = vec4(0,0,0,1);
    float scale = g;
    //float curvature = texture(foamMap,original_vertex.xz/scale).r;

    vec3 normal = normalize(normal_vector);

    /*
    float epsilon = 0.001;
    float h = 0,hx = 0,hz = 0;

    vec3 bump_normal = vec3(0.0);
    for (int i = 0; i < 2; i++) {
        vec3 vTexCoord3D;
        if (i == 0) {
            vTexCoord3D = vTexCoord3DSmall;
        }else{
            vTexCoord3D = vTexCoord3DLarge;
        }
        
        h += snoise(vTexCoord3D);
        hx += snoise(vTexCoord3D+vec3(epsilon,0.0,0.0));
        hz += snoise(vTexCoord3D+vec3(0.0,0.0,epsilon));

    }


    if (curvature > 0.15 && h > 0 ) {
        vec3 n = vec3(0.0,h,0.0);
        vec3 nx = vec3(epsilon,hx,0.0);
        vec3 nz = vec3(0.0,hz,epsilon);
        vec3 v1 = normalize(nx-n);
        vec3 v2 = normalize(nz-n);
        
        vec3 norm = cross(v1,v2);
        
        bump_normal += normalize(norm - vec3(0.0,0.0,1.0));
        fragColor  += h*0.6*vec4(1.0,0.9,0.85,0.0);

    }
    
    vec3 normal = normalize(normal_vector+30*bump_normal);
    */
    

    vec3 normal1         = normalize(normal);
    vec3 light_vector1   = normalize(light_vector);
    vec3 halfway_vector1 = normalize(halfway_vector);
 
    vec4 c = vec4(1,1,1,1);

    vec4 emissive_color = vec4(1.0, 1.0, 1.0,  1.0);
    vec4 base_ambient   = vec4(0.00,0.56,0.97,1);
    vec4 ambient_color  = base_ambient+0.03*v.y*vec4(0.0,0.6,1.0,1.0);
    vec4 diffuse_color  = vec4(0.00,0.80,0.97,1);
    vec4 specular_color = vec4(1.0,1.0,1.0,1.0);


    float ambient_contribution  = 0.80;
    float diffuse_contribution  = 0.30;
    float specular_contribution = 1.80;
 
    float d = dot(normal1, light_vector1);
    bool facing = d > 0.0;
 
    fragColor += ambient_color  * ambient_contribution  * c +
            diffuse_color  * diffuse_contribution  * c * max(d, 0) +
                    (facing ?
            specular_color * specular_contribution * c * max(pow(dot(normal1, halfway_vector1), 120.0), 0.0) :
            vec4(0.0, 0.0, 0.0, 0.0));

    

	//disabling foam calculation for now..
    float curvature = texture(normalMap,original_vertex.xz/scale).a;
    if (curvature > 0.1 ) {
        fragColor += 20*curvature*curvature*vec4(curvature,curvature,curvature,1.0);
    }

    //float z = 0.3*gl_FragCoord.z;
    //fragColor = vec4(z,z,z,1.0);
}



























