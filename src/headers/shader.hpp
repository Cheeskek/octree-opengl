#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <vector>

GLuint loadShader(const char *vertexFilePath, GLuint _shaderType);

GLuint createShaderProgram(std::vector<GLuint> shaderIDs);

#endif
