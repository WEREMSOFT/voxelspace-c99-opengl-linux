#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "imageData/imageData.h"

typedef struct
{
    unsigned int textureId;
    unsigned int VAO;
    unsigned int shaderProgram;
    PointI mousePosition;
    bool mouseRightDown;
    GLFWwindow *window;
    ImageData imageData;
} Graphics;

Graphics graphicsCreate(int width, int height);
void graphicsSwapBuffers(Graphics this);
void graphicsDestroy(Graphics this);
void graphicsUpdateMouseCoordinates(Graphics *this);
#endif