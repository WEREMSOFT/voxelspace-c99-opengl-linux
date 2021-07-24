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

typedef struct
{
    pthread_t handler;
    Program this;
    PointI cameraPosition;
    double lookingAngle;
    int startColumn;
    int endColumn;
    ImageData imageData;
} PthreadInfo;

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
    PointI cameraPosition = {0};
    double deltaTime;
    double angle = 0.0;

    PthreadInfo pthreads[PTHREAD_COUNT] = {0};

    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        pthreads[i].this = this;
        pthreads[i].imageData.bufferSize = this.graphics.imageData.bufferSize / PTHREAD_COUNT;
        pthreads[i].imageData.data = (Color *)malloc(pthreads[i].imageData.bufferSize);
        pthreads[i].imageData.size.x = this.graphics.imageData.size.x / PTHREAD_COUNT;
        pthreads[i].imageData.size.y = this.graphics.imageData.size.y;
        pthreads[i].startColumn = i * pthreads[i].imageData.size.x;
        pthreads[i].endColumn = pthreads[i].startColumn + pthreads[i].imageData.size.y;
    }

    double lastUpdate = 0;

    PointI lastCameraPosition = cameraPosition;
    double lastAngle = angle;

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
            angle += this.angularSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_D) == GLFW_PRESS)
            angle -= this.angularSpeed * deltaTime;

        if (cameraPosition.x != lastCameraPosition.x || cameraPosition.y != lastCameraPosition.y || angle != lastAngle)
        {
            this.levelOfDetail = 0.05;
        }
        else
        {
            this.levelOfDetail = 0.01;
        }

        for (int i = 0; i < PTHREAD_COUNT; i++)
        {
            pthreads[i].this = this;
            pthreads[i].cameraPosition = cameraPosition;
            pthreads[i].lookingAngle = angle;
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
        lastCameraPosition = cameraPosition;
        lastAngle = angle;
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
    PthreadInfo *info = (PthreadInfo *)params;

    double angle = info->lookingAngle;

    PointF pLeft = {0};
    PointF pRight = {0};

    // precalculate viewing angle parameters
    double sinphi = sinf(angle);
    double cosphi = cosf(angle);

    double dz = 1.0;

    int maxHeight[info->imageData.size.x];

    for (int i = 0; i < info->imageData.size.x; i++)
    {
        maxHeight[i] = info->this.graphics.imageData.size.y - 1;
    }

    double z = 1.0;
    imClear(info->imageData);
    while (z < info->this.distance)
    {

        pLeft = (PointF){(-cosphi - sinphi) * z + info->cameraPosition.x, (sinphi - cosphi) * z + info->cameraPosition.y};
        pRight = (PointF){(cosphi - sinphi) * z + info->cameraPosition.x, (-sinphi - cosphi) * z + info->cameraPosition.y};

        double dx = (pRight.x - pLeft.x) / info->this.graphics.imageData.size.x + (info->startColumn / info->this.graphics.imageData.size.x);
        double dy = (pRight.y - pLeft.y) / info->this.graphics.imageData.size.y;

        pLeft.x += dx * info->startColumn;
        pLeft.y += dy * info->startColumn;

        for (int i = 0; i < info->imageData.size.x; i++)
        {
            PointU mappedPoint = pointFToPointU(pLeft);

            unsigned int position = (mappedPoint.x + mappedPoint.y * info->this.heightMap.imageData.size.x);
            position %= MODULE;
            Color colorMapColor = info->this.colorMap.imageData.data[position];

            double colorRatio = (info->this.distance - z) / info->this.distance;

            colorMapColor.r *= colorRatio;
            colorMapColor.g *= colorRatio;
            colorMapColor.b *= colorRatio;

            Color heightMapColor = info->this.heightMap.imageData.data[position];
            int heightOnScreen = (info->this.height - heightMapColor.r) / (float)z * info->this.scale.y + info->this.horizon;

            heightOnScreen = MIN(MAX(heightOnScreen, 0), info->imageData.size.y);
            drawVerticalLine(info->imageData, (PointI){i, heightOnScreen}, colorMapColor, maxHeight[i]);
            pLeft.x += dx;
            pLeft.y += dy;
            maxHeight[i] = MIN(heightOnScreen, maxHeight[i]);
        }
        z += dz;
        dz += info->this.levelOfDetail;
    }

    return NULL;
}