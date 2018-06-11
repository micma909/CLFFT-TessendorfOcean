#ifndef __CLWL__GLStuff__
#define __CLWL__GLStuff__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glsl.h"
#include <iostream>

class GLWindow {
public:
    
    GLWindow();
    ~GLWindow();

    int init(int window_width, int window_height,
             GLfloat *verts,GLsizei v_size,
             GLuint *ind,GLsizei i_size);
    void compileShaders(char *vert,char *frag);
    void run();
    
    static void mouse_pressed(GLFWwindow *window,int ,int ,int);
    static void mouse_pos(GLFWwindow *window,double,double);
    static void mouse_scroll(GLFWwindow *window,double,double);

    void x_rot(float x);
    void y_rot(float y);

	float x_pan() const;
    float y_pan() const;
    
    float x_rot() const;
    float y_rot() const;
    float zoom() const;

public:
public:

    
    GLFWwindow* window;
    GLuint vao;
    GLuint vbo,nbo,ibo;
    GLuint vbo2;

    void (*mainLoop)(const GLWindow &stuff);

};
#endif /* defined(__CLWL__GLStuff__) */
