#include "hittable.h"

void hitRecordSetFaceNormal(HitRecord *this, Ray ray, Vec3 outWardNormal)
{
    this->frontFace = vec3Dot(ray.direction, outWardNormal) < 0;
    this->normal = this->frontFace ? outWardNormal : vec3Mult(outWardNormal, -1);
}
