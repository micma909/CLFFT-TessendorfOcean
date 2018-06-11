
#ifndef GLHelper_H
#define GLHelper_H
extern "C"
int CheckGLError(const char *file, int line);

extern "C"
GLuint generateImage2D(GLfloat *pixels,int W,int H);

#define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)
//#undef CHECK_GL_ERROR
//#define CHECK_GL_ERROR()
#endif