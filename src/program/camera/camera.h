#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "../vec3/vec3.h"

typedef struct
{
    float viewPortHeight;
    float viewPortWidth;
    float focalLength;
    double aspectRatio;
    Point3 origin, horizontal, vertical, lowerLeft;
} Camera;

Camera cameraCreate(double aspectRatio, Point3 origin);

#endif