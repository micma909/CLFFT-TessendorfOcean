#version 120

varying vec3 Normal;
varying float height;

void main(void){
    Normal = gl_Normal;
    height = gl_Vertex.y; 
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
}