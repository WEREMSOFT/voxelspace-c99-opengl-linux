#include "vec3.h"
#include "../utils/utils.h"
#include "../core/graphics/imageData/imageData.h"
#include <math.h>

Vec3 vec3Create(double x, double y, double z)
{
    Vec3 this = {x, y, z};
    return this;
}

Vec3 vec3Substract(Vec3 this, Vec3 other)
{
    return (Vec3){this.x - other.x, this.y - other.y, this.z - other.z};
}

Vec3 vec3AddScalarF(Vec3 this, double pScalar)
{
    return (Vec3){this.x + pScalar, this.y + pScalar, this.z + pScalar};
}

Vec3 vec3Add(Vec3 this, Vec3 other)
{
    return (Vec3){this.x + other.x, this.y + other.y, this.z + other.z};
}

Vec3 vec3Mult(Vec3 this, double scalar)
{
    return (Vec3){this.x * scalar, this.y * scalar, this.z * scalar};
}

double vec3Length(Vec3 this)
{
    return sqrtf(this.x * this.x + this.y * this.y + this.z * this.z);
}

double vec3LengthSqurd(Vec3 this)
{
    return this.x * this.x + this.y * this.y + this.z * this.z;
}

double vec3Dot(Vec3 this, Vec3 other)
{
    return this.x * other.x +
           this.y * other.y +
           this.z * other.z;
}

Vec3 vec3Cross(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

Vec3 vec3MultElem(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.x * b.x,
        a.y * b.y,
        a.z * b.z};
}

Vec3 vec3Unit(Vec3 this)
{
    return vec3Mult(this, 1 / vec3Length(this));
}

Vec3 v3Random(unsigned int *seed)
{
    return (Vec3){randomDouble(seed), randomDouble(seed), randomDouble(seed)};
}

Vec3 vec3RandomMinMax(double pMin, double pMax, unsigned int *seed)
{
    return (Vec3){randomDoubleMinMax(pMin, pMax, seed), randomDoubleMinMax(pMin, pMax, seed), randomDoubleMinMax(pMin, pMax, seed)};
}

bool vec3IsNearZero(Vec3 this)
{
    double s = 1e-8;
    return (fabs(this.x) < s) && (fabs(this.y) < s) && (fabs(this.z) < s);
}

Vec3 vec3Reflect(Vec3 this, Vec3 pNormal)
{
    return vec3Substract(this, vec3Mult(pNormal, 2 * vec3Dot(this, pNormal)));
}
