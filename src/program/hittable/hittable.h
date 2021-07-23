#ifndef __HITTABLE_H__
#define __HITTABLE_H__

#include <stdbool.h>
#include "../core/graphics/imageData/imageData.h"
#include "../ray/ray.h"

typedef bool (*MaterialScatterFunctionPointer)();

typedef struct
{
    Point3 point;
    Vec3 normal;
    Vec3 materialColor;
    double materialFuzz;
    MaterialScatterFunctionPointer materialScatterFunc;
    double t;
    bool frontFace;
} HitRecord;

void hitRecordSetFaceNormal(HitRecord *this, Ray ray, Vec3 outWardNormal);

#endif