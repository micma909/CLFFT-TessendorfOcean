#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Geometry.h"

namespace Geometry {
    void computeVertexStripSizes(int x,int y,GLsizei &verticies_size,GLsizei &index_size){
        verticies_size = (x+1)*(y+1)*3;
        index_size = x*y*6;
    }
    void generateVertexStrip(int x,int y,GLfloat *verticies, GLuint *indicies,bool normalized ){
        //Create vertices
        GLfloat scalex = 1.f/(GLfloat)x;
        GLfloat scaley = 1.f/(GLfloat)y;
        for (int j = 0; j < y+1; j++) {
            for (int i = 0; i < x+1; i++) {
                if(normalized){
                    verticies[3*i+3*(x+1)*j]   = (GLfloat)i*scalex-0.5f;
                    verticies[3*i+3*(x+1)*j+1] = 0;
                    verticies[3*i+3*(x+1)*j+2] = (GLfloat)j*scaley-0.5f;
                }else{
                    verticies[3*i+3*(x+1)*j] = i;
                    verticies[3*i+3*(x+1)*j+1] = 0.0;
                    verticies[3*i+3*(x+1)*j+2] = j;
                }

            }
        }
        
        //Connect vertices
        for (int j = 0; j < y; j++) {
            for (int i = 0; i < x; i++) {
                indicies[6*i+6*j*x] = i+(x+1)*j;
                indicies[6*i+6*j*x+1] = (i+1)+(x+1)*j;
                indicies[6*i+6*j*x+2] = i+(x+1)*(j+1);
                
                indicies[6*i+6*j*x+3] = (i+1)+(x+1)*j;
                indicies[6*i+6*j*x+4] = (i+1)+(x+1)*(j+1);
                indicies[6*i+6*j*x+5] = i+(x+1)*(j+1);
            }
        }
        
    }
}

