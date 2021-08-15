#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include "core/graphics/graphics.h"
#include "core/sprite/sprite.h"
#include "camera/camera.h"
#include "core/array/array.h"

enum EnumMapTYpe
{
    MAP_SNOW_WATERY,
    MAP_FOREST_WATERY,
    MAP_JUNGLE,
    MAP_MOUNTAINS_DESERT,
    MAP_MOUNTAINS,
    MAP_DESERT,
    MAP_COUNT
};

struct Program;

typedef struct
{
    pthread_t handler;
    int startColumn;
    int endColumn;
    ImageData imageData;
    struct Program *program;
} PthreadInfo;

#define PTHREAD_COUNT 8

typedef struct Program
{
    Graphics graphics;
    Sprite colorMaps[MAP_COUNT];
    Sprite heightMaps[MAP_COUNT];

    double lookingAngle;
    PointI cameraPosition;
    float cameraSpeed;
    double angularSpeed;
    double levelOfDetail;
    int distance;
    float height;
    int horizon;
    PointF scale;
    unsigned int mapIndex;
    double stopwatchStart;
    double deltaTime;
    PointI lastCameraPosition;
    double lastAngle;
    PthreadInfo pthreads[PTHREAD_COUNT];
} Program;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif