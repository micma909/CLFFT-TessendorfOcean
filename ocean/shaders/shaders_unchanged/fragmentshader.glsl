#version 120

varying vec3 Normal;
varying float height;

void main (void){
    float angle = dot(Normal,vec3(0.0,1.0,0.0));
    angle = max(angle,0.0);
    gl_FragColor = (10.0*height)*vec4(0.0, 0.2, 0.1, 0.0)+vec4(0.0, 0.5, 0.6, 0.0)*angle;
}
