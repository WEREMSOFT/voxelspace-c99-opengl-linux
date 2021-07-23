#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "../graphics/imageData/imageData.h"
#include <stdbool.h>

typedef struct
{
    int frameWidth;
    int t;
    int currentFrame;
    int frameCount;
    float frameIncrement;
    float inverseFramerrate;
} Animation;

typedef struct
{
    bool animated;
    Animation animation;
    PointF position;
    ImageData imageData;
} Sprite;

Sprite spriteCreate(char *file);
void spriteDraw(Sprite this, ImageData imageData);
void spriteDrawClipped(Sprite this, ImageData imageData);
void spriteDrawTransparent(Sprite this, ImageData ImageData);
void spriteDrawTransparentClipped(Sprite this, ImageData imageData);
void spriteDrawTransparentAnimatedClipped(Sprite *thisP, ImageData imageData, double deltaTime);
void spriteDestroy(Sprite this);
Sprite spriteCreateCkeckerBoard(PointI size, int checkerWidth, Color color1, Color color2);

#endif
