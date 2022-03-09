#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>

GLuint loadShader(const char *vertexFilePath, GLuint _shaderType);

GLuint createShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID);

#endif
