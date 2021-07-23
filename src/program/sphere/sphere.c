#include "sphere.h"
#include <math.h>
#include "../core/array/array.h"

bool sphereHit(Sphere this, Ray ray, double tMin, double tMax, HitRecord *hitRecord)
{
    double a = vec3LengthSqurd(ray.direction);
    Vec3 oc = vec3Substract(ray.origin, this.center);
    double half_b = vec3Dot(oc, ray.direction);
    double c = vec3LengthSqurd(oc) - this.radious * this.radious;

    double discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
        return false;
    double sqrtd = sqrtf(discriminant);

    // Find the nearest root that lies in the acceptable range.
    double root = (-half_b - sqrtd) / a;
    if (root < tMin || tMax < root)
    {
        root = (-half_b + sqrtd) / a;
        if (root < tMin || tMax < root)
            return false;
    }

    hitRecord->materialColor = this.materialColor;
    hitRecord->materialFuzz = this.materialFuzz;
    hitRecord->t = root;
    hitRecord->point = rayAt(ray, hitRecord->t);
    Vec3 outwardNormal = vec3Mult(vec3Substract(hitRecord->point, this.center), 1 / this.radious);
    hitRecordSetFaceNormal(hitRecord, ray, outwardNormal);

    return true;
}

bool spheresHit(Array *this, Ray ray, double tMin, double tMax, HitRecord *hitRecord)
{
    bool hitAnything = false;

    double closestSoFar = tMax;

    for (int i = 0; i < this->header.length; i++)
    {
        Sphere *tempSphere = arrayGetElementAt(this, i);
        HitRecord hitRecordSphere = {0};
        if (sphereHit(*tempSphere, ray, tMin, closestSoFar, &hitRecordSphere))
        {
            hitAnything = true;
            closestSoFar = hitRecordSphere.t;
            *hitRecord = hitRecordSphere;
            hitRecord->materialColor = tempSphere->materialColor;
            hitRecord->materialScatterFunc = tempSphere->materialScatterFunc;
        }
    }

    return hitAnything;
}