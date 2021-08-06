#include "shader.h"
#include <glad/glad.h>

static int shaderCreateFromFile(const char *fileName, unsigned int *vertexShader, int shaderType)
{
    int returnValue = 0;
    size_t fileSize = 0;
    char *shaderCode = NULL;
    *vertexShader = glCreateShader(shaderType);

    FILE *fp = fopen(fileName, "r");

    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        returnValue = -1;
        goto error_handler;
    }

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        perror("Error obtaining the file size ");
        returnValue = -1;
        goto error_handler;
    }

    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    shaderCode = malloc(fileSize + 1 * sizeof(char));
    shaderCode[fileSize] = 0;

    if (shaderCode == NULL)
    {
        returnValue = -1;
        goto error_handler;
    }

    if (fread(shaderCode, sizeof(char), fileSize, fp) != fileSize)
    {
        fprintf(stderr, "Error opening shader %s\n", fileName);
        exit(-1);
    }

    glShaderSource(*vertexShader, 1, (const char **)&shaderCode, NULL);

    glCompileShader(*vertexShader);

    int shaderCompilationSuccess;
    char infoLog[512];

    glGetShaderiv(*vertexShader, GL_COMPILE_STATUS, &shaderCompilationSuccess);

    if (!shaderCompilationSuccess)
    {
        glGetShaderInfoLog(*vertexShader, 512, NULL, infoLog);
        printf("%s::%s : vertex shader compilation failed. Error %s\n", __FILE__, __func__, infoLog);
        returnValue = -1;
        goto error_handler;
    }

error_handler:
    if (shaderCode != NULL)
        free(shaderCode);
    if (fp != NULL)
        fclose(fp);

    return returnValue;
}

unsigned int shaderProgramCreateFromFiles(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    unsigned int vs, fs;

    shaderCreateFromFile(vertexShaderPath, &vs, GL_VERTEX_SHADER);
    shaderCreateFromFile(fragmentShaderPath, &fs, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    if (vs == 0 || fs == 0)
    {
        fprintf(stderr, "error loading shadfer files\n");
        exit(-1);
    }
    glLinkProgram(shaderProgram);

    int success;

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("%s::%s - Error linking shader program: %s\n", __FILE__, __func__, infoLog);
        exit(-1);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return shaderProgram;
}

void shaderSetUniformValueMatrix4F(unsigned int shaderProgram, char *uniformName, const float *value)
{
    glUseProgram(shaderProgram);
    int loc = glGetUniformLocation(shaderProgram, uniformName);
    glUniformMatrix4fv(loc, 1, GL_FALSE, value);
}

void shaderSetUniformValueMatrix3F(unsigned int shaderProgram, char *uniformName, const float *value)
{
    glUseProgram(shaderProgram);
    int loc = glGetUniformLocation(shaderProgram, uniformName);
    glUniformMatrix3fv(loc, 1, GL_FALSE, value);
}

void shaderSetUniformValue3F(unsigned int shaderProgram, char *uniformName, const float *value)
{
    glUseProgram(shaderProgram);
    int location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform3f(location, value[0], value[1], value[2]);
}

void shaderSetUniformValueF(unsigned int shaderProgram, char *uniformName, const float value)
{
    glUseProgram(shaderProgram);
    int location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform1f(location, value);
}

void shaderSetUniformValueI(unsigned int shaderProgram, char *uniformName, const int value)
{
    glUseProgram(shaderProgram);
    int location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform1i(location, value);
}