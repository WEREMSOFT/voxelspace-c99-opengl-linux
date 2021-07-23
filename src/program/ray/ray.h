#ifndef __RAY_H__
#define __RAY_H__
#include "../vec3/vec3.h"

typedef struct
{
    Point3 origin, direction;
} Ray;

Ray rayCreate(Point3 origin, Point3 direction);
Point3 rayAt(Ray this, double t);
#endif