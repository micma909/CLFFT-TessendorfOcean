#ifndef __Shader__
#define __Shader__
#include <GL/glew.h>
#include <string>
#include <map>

class Shader{
private:
	GLuint glProgram;
    
    std::string v_source;
	std::string f_source;
    std::string c_source;
    bool hasComputeShader;
    //chached uniforms
    std::map<std::string,GLint> uniforms,attributes;
    
protected:
	std::string source(const std::string &src_file);
	void printShaderInfoLog(GLuint obj);
	void printProgramInfoLog(GLuint obj);
    GLuint addShader(GLenum shaderType,const std::string &source);
    void deleteShader(GLuint shader);
    void checkCompileStatus(GLuint& shader);
public:
    Shader(const std::string &v, const std::string &f);
    Shader(const std::string &v, const std::string &f, const std::string &c);
	void createProgram();
	GLuint program()  { return glProgram;}
    
    GLint uniform(const std::string &name);
    GLint attribute(const std::string &name);

};
#endif