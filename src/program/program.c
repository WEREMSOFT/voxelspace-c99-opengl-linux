#include "program.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define _GNU_SOURCE
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "core/input/input.h"
#include "utils/utils.h"
#include "sphere/sphere.h"
#include "vec3/vec3.h"

#define PTHREAD_COUNT 8

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#ifdef ARRAY_MALLOC
#undef ARRAY_MALLOC
#endif

#ifdef ARRAY_REALLOC
#undef ARRAY_REALLOC
#endif

#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "core/array/array.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void imRenderVoxelSpace(Program this, PointI pPoint, double angle);

Program programCreate()
{
    Program this = {0};
    float aspectRatio = 32.0 / 9.0;
    int wideSize = 800;
    this.graphics = graphicsCreate(wideSize, wideSize / aspectRatio);

    glfwSwapInterval(0);

    this.sound = soundCreate();
    Soloud_setGlobalVolume(this.sound.soloud, 0);

    this.heightMap = spriteCreate("assets/height.png");
    this.heightMap.position.x = this.heightMap.imageData.size.x;
    this.colorMap = spriteCreate("assets/color.png");

    this.height = 50;
    this.horizon = 50;
    this.scale.y = 300;
    this.distance = 50000;
    this.cameraSpeed = 2500.0;
    return this;
}

void programMainLoop(Program this)
{
    PointI cameraPosition = {0};
    double deltaTime = 0;
    double angle = 0.0;

#define ANGULAR_SPEED 10.0

    double lastUpdate = 0;

    while (glfwGetKey(this.graphics.window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        double deltaTime = glfwGetTime() - lastUpdate;
        lastUpdate = glfwGetTime();
        if (glfwGetKey(this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cameraPosition.x += this.cameraSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cameraPosition.x -= this.cameraSpeed * deltaTime;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
            cameraPosition.y -= this.cameraSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
            cameraPosition.y += this.cameraSpeed * deltaTime;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_I) == GLFW_PRESS)
            this.height += this.cameraSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_O) == GLFW_PRESS)
            this.height -= this.cameraSpeed * deltaTime;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_A) == GLFW_PRESS)
            angle += ANGULAR_SPEED * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_D) == GLFW_PRESS)
            angle -= ANGULAR_SPEED * deltaTime;

        imClear(this.graphics.imageData);
        imRenderVoxelSpace(this, cameraPosition, angle);

        printFPS(this.graphics, getDeltaTime());

        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }
}

void programDestroy(Program this)
{
    spriteDestroy(this.heightMap);
    spriteDestroy(this.colorMap);
    graphicsDestroy(this.graphics);
    soundDestroy(this.sound);
}

void drawVerticalLine(ImageData this, PointI start, Color color, int maxHeight)
{
    for (int i = start.y; i < maxHeight; i++)
    {
        start.y = i;
        imPutPixel(this, start, color);
    }
}
#define MODULE 1048576

void imRenderVoxelSpace(Program this, PointI pPoint, double angle)
{
    PointF pLeft = {0};
    PointF pRight = {0};

    // precalculate viewing angle parameters
    double sinphi = sinf(angle);
    double cosphi = cosf(angle);

    double dz = 1.0;

    int maxHeight[this.graphics.imageData.size.x];

    for (int i = 0; i < this.graphics.imageData.size.x; i++)
    {
        maxHeight[i] = this.graphics.imageData.size.y - 1;
    }

    // for (int z = 1; z < this.distance; z++)
    double z = 1.0;
    while (z < this.distance)
    {

        pLeft = (PointF){(-cosphi - sinphi) * z + pPoint.x, (sinphi - cosphi) * z + pPoint.y};
        pRight = (PointF){(cosphi - sinphi) * z + pPoint.x, (-sinphi - cosphi) * z + pPoint.y};

        double dx = (pRight.x - pLeft.x) / this.graphics.imageData.size.x;
        double dy = (pRight.y - pLeft.y) / this.graphics.imageData.size.y;

        for (int i = 0; i < this.graphics.imageData.size.x; i++)
        {
            PointU mappedPoint = pointFToPointU(pLeft);

            unsigned int position = (mappedPoint.x + mappedPoint.y * this.heightMap.imageData.size.x);
            position %= MODULE;
            Color colorMapColor = this.colorMap.imageData.data[position];
            Color heightMapColor = this.heightMap.imageData.data[position];
            int heightOnScreen = (this.height - heightMapColor.r) / (float)z * this.scale.y + this.horizon;

            heightOnScreen = MIN(MAX(heightOnScreen, 0), this.graphics.imageData.size.y);
            drawVerticalLine(this.graphics.imageData, (PointI){i, heightOnScreen}, colorMapColor, maxHeight[i]);
            pLeft.x += dx;
            pLeft.y += dy;
            maxHeight[i] = MIN(heightOnScreen, maxHeight[i]);
        }
        z += dz;
        dz += .005;
    }
}
