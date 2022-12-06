#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Inspired by the provided code in the practicals of the course
class Shader
{
private:
    GLuint compileShader(std::string shaderCode, GLenum shaderType);
    GLuint compileProgram(GLuint vertexShader, GLuint fragmentShader);

public:
    GLuint ID;

    Shader(const char *vertexPath, const char *fragmentPath);
    Shader(std::string vShaderCode, std::string fShaderCode);
    void use();
    void setInteger(const GLchar *name, GLint value);
    void setFloat(const GLchar *name, GLfloat value);
    void setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z);
    void setVector3f(const GLchar *name, const glm::vec3 &value);
    void setMatrix4(const GLchar *name, const glm::mat4 &matrix);
};

#endif