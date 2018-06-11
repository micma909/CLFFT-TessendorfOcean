
#ifndef GLHelper_H
#define GLHelper_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#if __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/GL.h>
#endif


static const char* GLErrorString(GLenum errorCode){
    static const struct {
        GLenum code;
        const char *string;
    } errors[]=
    {
        /* GL */
        {GL_NO_ERROR, "no error"},
        {GL_INVALID_ENUM, "invalid enumerant"},
        {GL_INVALID_VALUE, "invalid value"},
        {GL_INVALID_OPERATION, "invalid operation"},
        {GL_STACK_OVERFLOW, "stack overflow"},
        {GL_STACK_UNDERFLOW, "stack underflow"},
        {GL_OUT_OF_MEMORY, "out of memory"},
        
        {0, NULL }
    };
    
    int i;
    
    for (i=0; errors[i].string; i++)
    {
        if (errors[i].code == errorCode)
        {
            return errors[i].string;
        }
    }
    
    return NULL;
}

extern "C"
int CheckGLError(const char *file, int line){
    GLenum glErr;
    int    retCode = 0;
    
    glErr = glGetError();
    if (glErr != GL_NO_ERROR){
        const GLubyte* sError = gluErrorString(glErr);
        
        if (sError)
			std::cout << "GL Error #" << glErr << "(" << sError << ") " << " in File " << file << " at line: " << line << std::endl;
        else
		    std::cout << "GL Error #" << glErr << " (no message available)" << " in File " << file << " at line: " << line << std::endl;
        
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


extern "C"
GLuint generateImage2D(GLfloat *pixels,int W,int H){
    GLuint image;
    glGenTextures(1, &image);
    glBindTexture(GL_TEXTURE_2D, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, W, H, 0, GL_RGBA, GL_FLOAT, pixels);
    return image;
}


#define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)


#endif