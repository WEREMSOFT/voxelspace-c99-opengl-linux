#include "imageData.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

extern char fonts[][5];

void imPutPixel(ImageData this, PointI point, Color color)
{
    int position = (point.x + point.y * this.size.x);
    this.data[position] = color;
}

inline Color imGetPixel(ImageData this, PointU point)
{
    int position = point.x + point.y * this.size.x;
    return this.data[position];
}

void imDrawCircle(ImageData this, PointI center, double radious, Color color)
{
    for (int i = center.x - radious; i <= center.x + radious; i++)
    {
        for (int j = center.y - radious; j <= center.y + radious; j++)
        {
            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                imPutPixel(this, (PointI){i, j}, color);
        }
    }
}

void imDrawSquare(ImageData this, PointI topLeftCorner, PointI size, Color color)
{
    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {
        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {
            if (j == topLeftCorner.y || j == topLeftCorner.y + size.y || i == topLeftCorner.x || i == topLeftCorner.x + size.x)
                imPutPixel(this, (PointI){i, j}, color);
        }
    }
}

void imDrawCircleFill(ImageData this, PointI center, double radious, Color color)
{
    for (int i = center.x - radious; i <= center.x + radious; i++)
    {
        for (int j = center.y - radious; j <= center.y + radious; j++)
        {
            if (floor(sqrt(pow(center.x - i, 2) + pow(center.y - j, 2))) == radious)
                imPutPixel(this, (PointI){i, j}, color);
        }
    }
}

void imClear(ImageData this)
{
    memset(this.data, 0, this.bufferSize);
}

void imClearTransparent(ImageData this)
{
    const int limit = this.size.x * this.size.y;
    for (int i = 0; i < limit; i++)
    {
        this.data[i] = (Color){0xFF, 0, 0xFF};
    }
}

void imClearColor(ImageData this, Color color)
{
    const int limit = this.size.x * this.size.y;
    for (int i = 0; i < limit; i++)
    {
        this.data[i] = color;
    }
}

void imDrawCharacter(ImageData this, PointI topLeftCorner, unsigned int letter, Color color)
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j <= 8; j++)
        {
            if (fonts[letter][i] & (0b1000000 >> j))
                imPutPixel(this, (PointI){topLeftCorner.x + j, topLeftCorner.y + i}, color);
        }
    }
}

void imPrintFontTest(ImageData this)
{
    for (int i = 0; i < 255; i++)
    {
        imDrawCharacter(this, (PointI){i * 6, 100}, i, (Color){0xff, 0xff, 0xff});
    }
}

void imPrintString(ImageData this, PointI topLeftCorner, char *string, Color color)
{
    size_t stringLen = strlen(string);
    for (size_t i = 0; i < stringLen; i++)
    {
        int charOffset = string[i];
        imDrawCharacter(this, (PointI){topLeftCorner.x + i * 6, topLeftCorner.y}, charOffset, color);
    }
}

void imDrawLine(ImageData this, PointI pointA, PointI pointB, Color color)
{

    int dx = abs(pointB.x - pointA.x), sx = pointA.x < pointB.x ? 1 : -1;
    int dy = abs(pointB.y - pointA.y), sy = pointA.y < pointB.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        imPutPixel(this, pointA, color);
        if (pointA.x == pointB.x && pointA.y == pointB.y)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            pointA.x += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            pointA.y += sy;
        }
    }
}

void imDrawSquareFill(ImageData this, PointI topLeftCorner, PointI size, Color color)
{
    for (int i = topLeftCorner.x; i <= topLeftCorner.x + size.x; i++)
    {
        for (int j = topLeftCorner.y; j <= topLeftCorner.y + size.y; j++)
        {
            imPutPixel(this, (PointI){i, j}, color);
        }
    }
}

inline PointI pointFToPointI(PointF source)
{
    return (PointI){(int)source.x, (int)source.y};
}

inline PointU pointIToPointU(PointI source)
{
    return (PointU){(unsigned int)abs(source.x), (unsigned int)abs(source.y)};
}

inline PointU pointFToPointU(PointF source)
{
    return (PointU){(unsigned int)abs((int)round(source.x)), (unsigned int)abs((int)round(source.y))};
}