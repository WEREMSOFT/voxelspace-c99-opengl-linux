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

#define MODULE 1048576

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#ifdef ARRAY_MALLOC
#undef ARRAY_MALLOC
#endif

#ifdef ARRAY_REALLOC
#undef ARRAY_REALLOC
#endif

#define DEMO_MODE

#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "core/array/array.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "core/queue/queue.h"

pthread_mutex_t mutexMain = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t referenceCounterLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condVar = PTHREAD_COND_INITIALIZER;
int refCounter = 0;
Queue taskQ = {0};

void *imRenderVoxelSpaceSlice();
static inline void processDemoMode(Program *this);
static inline void processInput(Program *this);
static inline void initThreads(Program *this);
static inline void initThreadInfo(Program *this);
static inline void enqueueTasks(Program *this);
static inline void waitForTasksToFinish(void);
static inline void drawSlicesIntoTexture(Program *this);

Program programCreate()
{
    Program this = {0};

    float aspectRatio = 4.0 / 3.0;
    int wideSize = 800;
    this.graphics = graphicsCreate(wideSize, wideSize / aspectRatio);

    glfwSwapInterval(0);

    char *heightMapNames[] = {
        "assets/snow-watery-height.png",
        "assets/forest-watery-height.png",
        "assets/height.png",
        "assets/mountains-desert-height.png",
        "assets/mountains-height.png",
        "assets/desert-height.png"};

    char *colorMapNames[] = {
        "assets/snow-watery-color.png",
        "assets/forest-watery-color.png",
        "assets/color.png",
        "assets/mountains-desert-color.png",
        "assets/mountains-color.png",
        "assets/desert-color.png"};

    for (int i = 0; i < MAP_COUNT; i++)
    {
        this.heightMaps[i] = spriteCreate(heightMapNames[i]);
        this.colorMaps[i] = spriteCreate(colorMapNames[i]);
    }

    this.stopwatchStart = glfwGetTime();

    this.height = 200;
    this.horizon = 50;
    this.scale.y = 400;
    this.distance = 4000;
    this.cameraSpeed = 250.0;
    this.angularSpeed = 0.5;
    this.levelOfDetail = 0.01;
    return this;
}

void programMainLoop(Program this)
{
    double lastUpdate = 0;
    bool demoMode = true;

    initThreads(&this);
    initThreadInfo(&this);

    this.lastCameraPosition = this.cameraPosition;
    this.lastAngle = this.lookingAngle;

    while (glfwGetKey(this.graphics.window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        this.deltaTime = glfwGetTime() - lastUpdate;
        lastUpdate = glfwGetTime();

        #ifdef DEMO_MODE
            processDemoMode(&this);
        #else
            processInput(&this);
        #endif

        enqueueTasks(&this);
        waitForTasksToFinish();
        drawSlicesIntoTexture(&this);

        printFPS(this.graphics, getDeltaTime());
        graphicsRender(this.graphics);
        glfwPollEvents();
        this.lastCameraPosition = this.cameraPosition;
        this.lastAngle = this.lookingAngle;
    }
}

void programDestroy(Program this)
{
    pthread_mutex_destroy(&mutexMain);
    pthread_mutex_destroy(&referenceCounterLock);
    for (int i = 0; i < MAP_COUNT; i++)
    {
        spriteDestroy(this.heightMaps[i]);
        spriteDestroy(this.colorMaps[i]);
    }
    graphicsDestroy(this.graphics);
}

void drawVerticalLine(ImageData this, PointI start, Color color, int maxHeight)
{
    for (int i = start.y; i < maxHeight; i++)
    {
        start.y = i;
        imPutPixel(this, start, color);
    }
}

void *imRenderVoxelSpaceSlice()
{
    PthreadInfo *this = NULL;

    while (true)
    {
        pthread_mutex_lock(&mutexMain);
        pthread_cond_wait(&condVar, &mutexMain);
        this = (PthreadInfo *)queueDequeue(&taskQ);
        pthread_mutex_unlock(&mutexMain);

        if (this != NULL)
        {
            PthreadInfo that = *this;
            double angle = that.program->lookingAngle;

            PointF pLeft = {0};
            PointF pRight = {0};

            // precalculate viewing angle parameters
            double sinphi = sinf(angle);
            double cosphi = cosf(angle);

            double dz = 1.0;

            int maxHeight[that.imageData.size.x];

            for (int i = 0; i < that.imageData.size.x; i++)
            {
                maxHeight[i] = that.program->graphics.imageData.size.y - 1;
            }

            double z = 1.0;
            imClear(that.imageData);
            while (z < that.program->distance)
            {

                pLeft = (PointF){(-cosphi - sinphi) * z + that.program->cameraPosition.x, (sinphi - cosphi) * z + that.program->cameraPosition.y};
                pRight = (PointF){(cosphi - sinphi) * z + that.program->cameraPosition.x, (-sinphi - cosphi) * z + that.program->cameraPosition.y};

                double dx = (pRight.x - pLeft.x) / that.program->graphics.imageData.size.x + (that.startColumn / that.program->graphics.imageData.size.x);
                double dy = (pRight.y - pLeft.y) / that.program->graphics.imageData.size.y;

                pLeft.x += dx * that.startColumn;
                pLeft.y += dy * that.startColumn;

                for (int i = 0; i < that.imageData.size.x; i++)
                {
                    PointU mappedPoint = pointFToPointU(pLeft);

                    unsigned int position = (mappedPoint.x + mappedPoint.y * that.program->colorMaps[that.program->mapIndex].imageData.size.x);

                    position %= MODULE;
                    Color colorMapColor = that.program->colorMaps[that.program->mapIndex].imageData.data[position];

                    double colorRatio = (that.program->distance - z) / that.program->distance;

                    colorMapColor.r *= colorRatio;
                    colorMapColor.g *= colorRatio;
                    colorMapColor.b *= colorRatio;

                    position = (mappedPoint.x + mappedPoint.y * that.program->heightMaps[that.program->mapIndex].imageData.size.x);
                    position %= that.program->heightMaps[that.program->mapIndex].imageData.bufferSize;
                    char heightMapColor = ((char *)that.program->heightMaps[that.program->mapIndex].imageData.data)[position];
                    int heightOnScreen = (that.program->height - heightMapColor) / (float)z * that.program->scale.y + that.program->horizon;

                    heightOnScreen = MIN(MAX(heightOnScreen, 0), that.imageData.size.y);
                    drawVerticalLine(that.imageData, (PointI){i, heightOnScreen}, colorMapColor, maxHeight[i]);
                    pLeft.x += dx;
                    pLeft.y += dy;
                    maxHeight[i] = MIN(heightOnScreen, maxHeight[i]);
                }
                z += dz;
                dz += that.program->levelOfDetail;
            }
            pthread_mutex_lock(&referenceCounterLock);
            refCounter--;
            pthread_mutex_unlock(&referenceCounterLock);
        }
    }
    return NULL;
}

static inline void processDemoMode(Program *this)
{
    this->cameraPosition.y += this->cameraSpeed * this->deltaTime;
    this->cameraPosition.x += this->cameraSpeed * this->deltaTime;
    this->lookingAngle -= this->angularSpeed * this->deltaTime;
    if (glfwGetTime() - this->stopwatchStart > 3.0)
    {
        this->mapIndex += 1;
        this->mapIndex %= MAP_COUNT;
        this->stopwatchStart = glfwGetTime();
    }
}

static inline void processInput(Program *this)
{
    if (glfwGetKey(this->graphics.window, GLFW_KEY_F1) == GLFW_PRESS)
        this->scale.x = this->scale.y += 10;
    else if (glfwGetKey(this->graphics.window, GLFW_KEY_F2) == GLFW_PRESS)
        this->scale.x = this->scale.y -= 10;

    if (glfwGetKey(this->graphics.window, GLFW_KEY_F3) == GLFW_PRESS)
        this->distance += 100;
    else if (glfwGetKey(this->graphics.window, GLFW_KEY_F4) == GLFW_PRESS)
        this->distance -= 100;

    if (glfwGetKey(this->graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
        this->cameraPosition.x += this->cameraSpeed * this->deltaTime;
    else if (glfwGetKey(this->graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        this->cameraPosition.x -= this->cameraSpeed * this->deltaTime;

    if (glfwGetKey(this->graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
        this->cameraPosition.y -= this->cameraSpeed * this->deltaTime;
    else if (glfwGetKey(this->graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
        this->cameraPosition.y += this->cameraSpeed * this->deltaTime;

    if (glfwGetKey(this->graphics.window, GLFW_KEY_I) == GLFW_PRESS)
        this->height += this->cameraSpeed * this->deltaTime;
    else if (glfwGetKey(this->graphics.window, GLFW_KEY_O) == GLFW_PRESS)
        this->height -= this->cameraSpeed * this->deltaTime;

    if (glfwGetKey(this->graphics.window, GLFW_KEY_A) == GLFW_PRESS)
        this->lookingAngle += this->angularSpeed * this->deltaTime;
    else if (glfwGetKey(this->graphics.window, GLFW_KEY_D) == GLFW_PRESS)
        this->lookingAngle -= this->angularSpeed * this->deltaTime;

    if (this->cameraPosition.x - this->lastCameraPosition.x + this->cameraPosition.y - this->lastCameraPosition.y + this->lookingAngle - this->lastAngle != 0)
    {
        this->levelOfDetail = 0.05;
    }
    else
    {
        this->levelOfDetail = 0.01;
    }

    if (isKeyJustPressed(this->graphics.window, GLFW_KEY_K))
    {
        this->mapIndex++;
        this->mapIndex %= MAP_COUNT;
    }

    if (isKeyJustPressed(this->graphics.window, GLFW_KEY_L))
    {
        this->mapIndex--;
        this->mapIndex %= MAP_COUNT;
    }
}

static inline void initThreads(Program *this)
{
    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        this->pthreads[i].program = this;
        pthread_create(&this->pthreads[i].handler, NULL, imRenderVoxelSpaceSlice, NULL);
    }
}

static inline void initThreadInfo(Program *this)
{
    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        this->pthreads[i].program = this;
        this->pthreads[i].imageData.bufferSize = this->graphics.imageData.bufferSize / PTHREAD_COUNT;
        this->pthreads[i].imageData.data = (Color *)malloc(this->pthreads[i].imageData.bufferSize);
        this->pthreads[i].imageData.size.x = this->graphics.imageData.size.x / PTHREAD_COUNT;
        this->pthreads[i].imageData.size.y = this->graphics.imageData.size.y;
        this->pthreads[i].startColumn = i * this->pthreads[i].imageData.size.x;
        this->pthreads[i].endColumn = this->pthreads[i].startColumn + this->pthreads[i].imageData.size.y;
    }
}

static inline void enqueueTasks(Program *this)
{
    pthread_mutex_lock(&mutexMain);
    {
        for (int i = 0; i < PTHREAD_COUNT; i++)
        {
            queueEnqueue(&taskQ, &this->pthreads[i]);
            pthread_cond_signal(&condVar);
        }
    }
    pthread_mutex_unlock(&mutexMain);
}

inline static void waitForTasksToFinish(void)
{
    int tempRefCounter = PTHREAD_COUNT;
    while (tempRefCounter > 0)
    {
        pthread_mutex_lock(&referenceCounterLock);
        tempRefCounter = refCounter;
        pthread_mutex_unlock(&referenceCounterLock);
    };
    refCounter = PTHREAD_COUNT;
}

inline static void drawSlicesIntoTexture(Program *this)
{
    Sprite tempSprite = {0};
    for (int i = 0; i < PTHREAD_COUNT; i++)
    {
        tempSprite.position.x = this->pthreads[i].startColumn;
        tempSprite.imageData = this->pthreads[i].imageData;
        spriteDraw(tempSprite, this->graphics.imageData);
    }
}