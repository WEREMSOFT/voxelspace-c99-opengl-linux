#ifndef __SHADER_H__
#define __SHADER_H__

#include <stdio.h>
#include <stdlib.h>

unsigned int shaderProgramCreateFromFiles(const char *vertexShaderPath, const char *fragmentShaderPath);
void shaderSetUniformValueMatrix4F(unsigned int shaderProgram, char *uniformName, const float *value);
void shaderSetUniformValueMatrix3F(unsigned int shaderProgram, char *uniformName, const float *value);
void shaderSetUniformValue3F(unsigned int shaderProgram, char *uniformName, const float *value);
void shaderSetUniformValueF(unsigned int shaderProgram, char *uniformName, const float value);
void shaderSetUniformValueI(unsigned int shaderProgram, char *uniformName, const int value);

#endif