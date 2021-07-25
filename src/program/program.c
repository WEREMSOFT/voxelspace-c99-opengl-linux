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
#include "vec3/vec3.h"

#define PTHREAD_COUNT 7
#define MODULE 1048576

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

void *imRenderVoxelSpaceSlice(void *params);

Program programCreate()
{
    Program this = {0};
    float aspectRatio = 16.0 / 9.0;
    int wideSize = 1024;
    this.graphics = graphicsCreate(wideSize, wideSize / aspectRatio);

    glfwSwapInterval(0);

    this.sound = soundCreate();
    Soloud_setGlobalVolume(this.sound.soloud, 0);

    this.heightMap = spriteCreate("assets/height.png");
    this.heightMap.position.x = this.heightMap.imageData.size.x;
    this.colorMap = spriteCreate("assets/color.png");

    this.height = 200;
    this.horizon = 50;
    this.scale.y = 400;
    this.distance = 10000;
    this.cameraSpeed = 500.0;
    this.angularSpeed = 1.5;
    this.levelOfDetail = 0.001;
    return this;
}

void programMainLoop(Program this)
{
    PthreadInfo pthreads[PTHREAD_COUNT] = {0};

    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        pthreads[i].imageData.bufferSize = this.graphics.imageData.bufferSize / PTHREAD_COUNT;
        pthreads[i].imageData.data = (Color *)malloc(pthreads[i].imageData.bufferSize);
        pthreads[i].imageData.size.x = this.graphics.imageData.size.x / PTHREAD_COUNT;
        pthreads[i].imageData.size.y = this.graphics.imageData.size.y;
        pthreads[i].startColumn = i * pthreads[i].imageData.size.x;
        pthreads[i].endColumn = pthreads[i].startColumn + pthreads[i].imageData.size.y;
    }

    double lastUpdate = 0;

    PointI lastCameraPosition = this.cameraPosition;
    double lastAngle = this.lookingAngle;

    while (glfwGetKey(this.graphics.window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        double deltaTime = glfwGetTime() - lastUpdate;
        lastUpdate = glfwGetTime();

        if (glfwGetKey(this.graphics.window, GLFW_KEY_F1) == GLFW_PRESS)
            this.scale.x = this.scale.y += 10;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_F2) == GLFW_PRESS)
            this.scale.x = this.scale.y -= 10;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_F3) == GLFW_PRESS)
            this.distance += 100;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_F4) == GLFW_PRESS)
            this.distance -= 100;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
            this.cameraPosition.x += this.cameraSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            this.cameraPosition.x -= this.cameraSpeed * deltaTime;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
            this.cameraPosition.y -= this.cameraSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
            this.cameraPosition.y += this.cameraSpeed * deltaTime;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_I) == GLFW_PRESS)
            this.height += this.cameraSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_O) == GLFW_PRESS)
            this.height -= this.cameraSpeed * deltaTime;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_A) == GLFW_PRESS)
            this.lookingAngle += this.angularSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_D) == GLFW_PRESS)
            this.lookingAngle -= this.angularSpeed * deltaTime;

        if (this.cameraPosition.x - lastCameraPosition.x + this.cameraPosition.y - lastCameraPosition.y + this.lookingAngle - lastAngle != 0)
        {
            this.levelOfDetail = 0.05;
        }
        else
        {
            this.levelOfDetail = 0.01;
        }

        for (int i = 0; i < PTHREAD_COUNT; i++)
        {
            pthreads[i].program = this;
            pthread_create(&pthreads[i].handler, NULL, imRenderVoxelSpaceSlice, &pthreads[i]);
        }

        Sprite tempSprite = {0};
        for (int i = 0; i < PTHREAD_COUNT; i++)
        {
            pthread_join(pthreads[i].handler, NULL);
            tempSprite.position.x = pthreads[i].startColumn;
            tempSprite.imageData = pthreads[i].imageData;
            spriteDraw(tempSprite, this.graphics.imageData);
        }

        printFPS(this.graphics, getDeltaTime());

        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
        lastCameraPosition = this.cameraPosition;
        lastAngle = this.lookingAngle;
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

void *imRenderVoxelSpaceSlice(void *params)
{
    PthreadInfo this = *(PthreadInfo *)params;

    double angle = this.program.lookingAngle;

    PointF pLeft = {0};
    PointF pRight = {0};

    // precalculate viewing angle parameters
    double sinphi = sinf(angle);
    double cosphi = cosf(angle);

    double dz = 1.0;

    int maxHeight[this.imageData.size.x];

    for (int i = 0; i < this.imageData.size.x; i++)
    {
        maxHeight[i] = this.program.graphics.imageData.size.y - 1;
    }

    double z = 1.0;
    imClear(this.imageData);
    while (z < this.program.distance)
    {

        pLeft = (PointF){(-cosphi - sinphi) * z + this.program.cameraPosition.x, (sinphi - cosphi) * z + this.program.cameraPosition.y};
        pRight = (PointF){(cosphi - sinphi) * z + this.program.cameraPosition.x, (-sinphi - cosphi) * z + this.program.cameraPosition.y};

        double dx = (pRight.x - pLeft.x) / this.program.graphics.imageData.size.x + (this.startColumn / this.program.graphics.imageData.size.x);
        double dy = (pRight.y - pLeft.y) / this.program.graphics.imageData.size.y;

        pLeft.x += dx * this.startColumn;
        pLeft.y += dy * this.startColumn;

        for (int i = 0; i < this.imageData.size.x; i++)
        {
            PointU mappedPoint = pointFToPointU(pLeft);

            unsigned int position = (mappedPoint.x + mappedPoint.y * this.program.heightMap.imageData.size.x);
            position %= MODULE;
            Color colorMapColor = this.program.colorMap.imageData.data[position];

            double colorRatio = (this.program.distance - z) / this.program.distance;

            colorMapColor.r *= colorRatio;
            colorMapColor.g *= colorRatio;
            colorMapColor.b *= colorRatio;

            Color heightMapColor = this.program.heightMap.imageData.data[position];
            int heightOnScreen = (this.program.height - heightMapColor.r) / (float)z * this.program.scale.y + this.program.horizon;

            heightOnScreen = MIN(MAX(heightOnScreen, 0), this.imageData.size.y);
            drawVerticalLine(this.imageData, (PointI){i, heightOnScreen}, colorMapColor, maxHeight[i]);
            pLeft.x += dx;
            pLeft.y += dy;
            maxHeight[i] = MIN(heightOnScreen, maxHeight[i]);
        }
        z += dz;
        dz += this.program.levelOfDetail;
    }

    return NULL;
}