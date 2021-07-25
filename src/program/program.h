#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include "core/graphics/graphics.h"
#include "core/sprite/sprite.h"
#include "sound/sound.h"
#include "camera/camera.h"
#include "core/array/array.h"

typedef struct
{
    Graphics graphics;
    Sound sound;
    Sprite colorMap;
    Sprite heightMap;

    double lookingAngle;
    PointI cameraPosition;
    float cameraSpeed;
    double angularSpeed;
    double levelOfDetail;
    int distance;
    float height;
    int horizon;
    PointF scale;
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