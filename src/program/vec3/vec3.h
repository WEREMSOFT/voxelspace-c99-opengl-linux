#ifndef __VEC3_H__
#define __VEC3_H__

#include <stdio.h>
#include <stdbool.h>
#include "../core/graphics/imageData/imageData.h"
typedef struct
{
    double x, y, z;
} Vec3;

typedef Vec3 Point3;

Vec3 vec3Create(double, double, double);
Vec3 vec3Substract(Vec3, Vec3);
Vec3 vec3Add(Vec3, Vec3);
Vec3 vec3Mult(Vec3, double);
double vec3Length(Vec3);
double vec3LengthSqurd(Vec3);
double vec3Dot(Vec3, Vec3);
Vec3 vec3Cross(Vec3, Vec3);
Vec3 vec3Unit(Vec3 this);
Vec3 v3Random(unsigned int *seed);
Vec3 vec3RandomMinMax(double pMin, double pMax, unsigned int *seed);
Vec3 vec3AddScalarF(Vec3 this, double pScalar);
bool vec3IsNearZero(Vec3 this);
Vec3 vec3Reflect(Vec3 this, Vec3 pNormal);
Vec3 vec3MultElem(Vec3 a, Vec3 b);
Color vec3ToColor(Vec3 this);
Vec3 colorToVec3(Color this);
#endif