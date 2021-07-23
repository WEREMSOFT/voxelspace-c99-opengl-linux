#ifndef __IMAGE_DATA_H__
#define __IMAGE_DATA_H__

typedef struct
{
    unsigned char r, g, b;
} Color;

typedef struct
{
    int x;
    int y;
} PointI;

typedef struct
{
    unsigned int x;
    unsigned int y;
} PointU;

typedef struct
{
    double x;
    double y;
} PointF;

typedef struct
{
    PointI size;
    int bufferSize;
    Color *data;
} ImageData;

void imPutPixel(ImageData this, PointI point, Color color);
void imDrawCircle(ImageData this, PointI center, double radious, Color color);
void imDrawCircleFill(ImageData this, PointI center, double radious, Color color);
void imDrawSquare(ImageData this, PointI topLeftCorner, PointI size, Color color);
void imClear(ImageData this);
void imClearColor(ImageData this, Color color);
void imClearTransparent(ImageData this);
void imDrawCharacter(ImageData this, PointI topLeftCorner, unsigned int letter, Color color);
void imPrintFontTest(ImageData this);
void imPrintString(ImageData this, PointI topLeftCorner, char *string, Color color);
void imDrawLine(ImageData this, PointI pointA, PointI pointB, Color color);
Color imGetPixel(ImageData this, PointU point);
PointI pointFToPointI(PointF);
PointU pointIToPointU(PointI);
PointU pointFToPointU(PointF source);

#endif