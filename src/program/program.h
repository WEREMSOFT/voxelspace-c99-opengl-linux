#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include "core/graphics/graphics.h"
#include "core/sprite/sprite.h"
#include "sound/sound.h"
#include "camera/camera.h"
#include "core/array/array.h"

enum EnumMapType
{
    MAP_SNOW_WATERY,
    MAP_FOREST_WATERY,
    MAP_MOUNTAINS_DESERT,
    MAP_MOUNTAINS,
    MAP_JUNGLE,
    MAP_DESERT,
    MAP_COUNT
};

typedef struct
{
    Graphics graphics;
    Sound sound;
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
    char colorMapNames[MAP_COUNT][50];
    char heightMapNames[MAP_COUNT][50];
} Program;

typedef struct
{
    pthread_t handler;
    int startColumn;
    int endColumn;
    ImageData imageData;
    Program program;
} PthreadInfo;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif