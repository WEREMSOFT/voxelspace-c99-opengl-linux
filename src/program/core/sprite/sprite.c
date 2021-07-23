#include "sprite.h"
#include <math.h>
#include <stb_image.h>

Sprite spriteCreate(char *file)
{
    Sprite this = {0};
    int nrChannels;
    this.imageData.data = (Color *)stbi_load(file, &this.imageData.size.x, &this.imageData.size.y, &nrChannels, 0);
    this.imageData.bufferSize = this.imageData.size.x * this.imageData.size.y * sizeof(Color);
    if (this.imageData.data == NULL)
    {
        fprintf(stderr, "Error loading file %s. Aborting.\n", file);
        exit(-1);
    }
    return this;
}

void spriteDraw(Sprite this, ImageData imageData)
{
    for (int i = 0; i < this.imageData.size.x; i++)
    {
        for (int j = 0; j < this.imageData.size.y; j++)
        {
            Color color = this.imageData.data[j * this.imageData.size.x + i];
            imPutPixel(imageData, (PointI){this.position.x + i, this.position.y + j}, color);
        }
    }
}

void spriteDrawClipped(Sprite this, ImageData imageData)
{
    int clippedWidth = fmin(this.imageData.size.x, fmax(0, this.imageData.size.x - (this.imageData.size.x + this.position.x - imageData.size.x)));
    int clippedHeight = fmin(this.imageData.size.y, fmax(0, this.imageData.size.y - (this.imageData.size.y + this.position.y - imageData.size.y)));
    int clippedX = this.position.x < 0 ? -this.position.x : 0;
    int clippedY = this.position.y < 0 ? -this.position.y : 0;
    for (int i = clippedX; i < clippedWidth; i++)
    {
        for (int j = clippedY; j < clippedHeight; j++)
        {
            Color color = this.imageData.data[j * this.imageData.size.x + i];
            imPutPixel(imageData, (PointI){this.position.x + i, this.position.y + j}, color);
        }
    }
}

void spriteDrawTransparent(Sprite this, ImageData imageData)
{
    for (int i = 0; i < this.imageData.size.x; i++)
    {
        for (int j = 0; j < this.imageData.size.y; j++)
        {
            Color color = this.imageData.data[j * this.imageData.size.x + i];
            if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                imPutPixel(imageData, (PointI){this.position.x + i, this.position.y + j}, color);
        }
    }
}

void spriteDrawTransparentClipped(Sprite this, ImageData imageData)
{
    int clippedWidth = fmin(this.imageData.size.x,
                            fmax(0, this.imageData.size.x - (this.imageData.size.x + this.position.x - imageData.size.x)));
    int clippedHeight = fmin(this.imageData.size.y,
                             fmax(0, this.imageData.size.y - (this.imageData.size.y + this.position.y - imageData.size.y)));
    int clippedX = this.position.x < 0 ? -this.position.x : 0;
    int clippedY = this.position.y < 0 ? -this.position.y : 0;
    for (int i = clippedX; i < clippedWidth; i++)
    {
        for (int j = clippedY; j < clippedHeight; j++)
        {
            Color color = this.imageData.data[j * this.imageData.size.x + i];
            if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                imPutPixel(imageData, (PointI){this.position.x + i, this.position.y + j}, color);
        }
    }
}

void spriteDestroy(Sprite this)
{
    free(this.imageData.data);
}

Sprite spriteCreateCkeckerBoard(PointI size, int checkerWidth, Color color1, Color color2)
{
    Sprite this = {0};
    Color currentColor = color1;
    this.imageData.data = malloc(sizeof(Color) * size.x * size.y);
    this.imageData.size = size;
    for (int y = 0; y < this.imageData.size.y; y++)
    {
        for (int x = 0; x < this.imageData.size.x; x++)
        {
            if ((y / checkerWidth + x / checkerWidth) % 2)
            {
                currentColor = color1;
            }
            else
            {
                currentColor = color2;
            }
            this.imageData.data[x + y * size.x] = currentColor;
        }
    }
    return this;
}

void spriteDrawTransparentAnimatedClipped(Sprite *thisP, ImageData imageData, double deltaTime)
{
    Sprite this = *thisP;

    int clippedWidth = fmin(this.animation.frameWidth,
                            fmax(0, this.animation.frameWidth - (this.animation.frameWidth + this.position.x -
                                                                 imageData.size.x)));
    int clippedHeight = fmin(this.imageData.size.y,
                             fmax(0, this.imageData.size.y - (this.imageData.size.y + this.position.y -
                                                              imageData.size.y)));
    int clippedX = this.position.x < 0 ? -this.position.x : 0;
    int clippedY = this.position.y < 0 ? -this.position.y : 0;

    thisP->animation.frameIncrement += deltaTime * 30;
    thisP->animation.currentFrame = this.animation.frameIncrement;
    thisP->animation.currentFrame %= this.animation.frameCount;

    for (int i = clippedX; i < clippedWidth; i++)
    {
        for (int j = clippedY; j < clippedHeight; j++)
        {
            Color color = this.imageData.data[j * this.imageData.size.x + i +
                                              this.animation.currentFrame * this.animation.frameWidth];
            if (!(color.r == 0xFF && color.b == 0xFF && color.g == 0))
                imPutPixel(imageData,
                           (PointI){this.position.x + i, this.position.y + j},
                           color);
        }
    }
}
