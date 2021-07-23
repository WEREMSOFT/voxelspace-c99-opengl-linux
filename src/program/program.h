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

    float cameraSpeed;
    int distance;
    float height;
    int horizon;
    PointF scale;
} Program;

Program programCreate();
void programMainLoop(Program this);
void programDestroy(Program this);

#endif