#include "graphics.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "../shader/shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

static void textureCreate(Graphics *this)
{

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    this->imageData.bufferSize = this->imageData.size.x * this->imageData.size.y * sizeof(Color);
    this->imageData.data = malloc(this->imageData.bufferSize);
    if (this->imageData.data == NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        exit(-1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imageData.size.x, this->imageData.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, this->imageData.data);
}

void textureCreateFromImage(Graphics *this, char *fileName)
{
    int width, height, nrChannels;

    glGenTextures(1, &this->textureId);
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    // Set texture wrapping filtering options.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(false);
    this->imageData.data = (Color *)stbi_load(fileName, &width, &height, &nrChannels, 0);

    if (this->imageData.data)
    {

        GLenum format = 0;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, this->imageData.data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("%s::%s - Error loading texture \n", __FILE__, __FUNCTION__);
        printf("Error: %s\n", stbi_failure_reason());
        exit(1);
    }
}

Graphics graphicsCreate(int width, int height)
{
    Graphics this = {0};
    this.imageData.size.x = width;
    this.imageData.size.y = height;

    glfwInit();
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this.window = glfwCreateWindow(mode->width, mode->height, "Frame Buffer", monitor, NULL);
    glfwMakeContextCurrent(this.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        exit(-1);
    }

    double ratioX = ((float)this.imageData.size.x / (float)this.imageData.size.y) / ((float)mode->width / (float)mode->height);
    double ratioY = 1.0;

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions       // texture coords
        ratioX * 1.0f, ratioY * 1.0f, 0.0f, 1.0f, 0.0f,   // top right
        ratioX * 1.0f, ratioY * -1.0f, 0.0f, 1.0f, 1.0f,  // bottom right
        ratioX * -1.0f, ratioY * -1.0f, 0.0f, 0.0f, 1.0f, // bottom left
        ratioX * -1.0f, ratioY * 1.0f, 0.0f, 0.0f, 0.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    textureCreate(&this);

    this.shaderProgram = shaderProgramCreateFromFiles("assets/shaders/default.vs", "assets/shaders/default.fs");
    glUseProgram(this.shaderProgram);

    unsigned int VBO, EBO;
    glGenVertexArrays(1, &this.VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(this.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glfwSetInputMode(this.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSetCursorPos(this.window, 0, 0);

    return this;
}

void graphicsSwapBuffers(Graphics this)
{
    // Update texture
    glBindTexture(GL_TEXTURE_2D, this.textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this.imageData.size.x, this.imageData.size.y, GL_RGB, GL_UNSIGNED_BYTE, this.imageData.data);

    // render container
    glBindVertexArray(this.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glFlush();
}

void graphicsDestroy(Graphics this)
{
    glfwSetWindowShouldClose(this.window, true);
    free(this.imageData.data);
    glfwDestroyWindow(this.window);
    glfwTerminate();
}

void graphicsUpdateMouseCoordinates(Graphics *this)
{
    int w, h;
    double mouseX, mouseY;
    glfwGetWindowSize(this->window, &w, &h);
    glfwGetCursorPos(this->window, &mouseX, &mouseY);
    this->mousePosition.x = mouseX + this->imageData.size.x / (float)w;
    this->mousePosition.y = mouseY + this->imageData.size.y / (float)h;
    this->mouseRightDown = glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_1);
}
