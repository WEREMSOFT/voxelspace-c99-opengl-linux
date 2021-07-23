#include "ray.h"
#include <math.h>
#include "../sphere/sphere.h"

Ray rayCreate(Point3 origin, Point3 direction)
{
    Ray this = (Ray){origin, direction};
    return this;
}

Point3 rayAt(Ray this, double t)
{
    return vec3Add(this.origin, vec3Mult(this.direction, t));
}
