#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "../vec3/vec3.h"
#include "../ray/ray.h"
#include "../hittable/hittable.h"

bool lambertianMaterialScatter(Vec3 albedo, Ray pRay, HitRecord *pHitRecord, Vec3 *pAtenuation, Ray *pScattered, unsigned int *seed);
bool metalMaterialScatter(Vec3 albedo, Ray pRay, HitRecord *hitRecord, Vec3 *atenuation, Ray *pScattered, unsigned int *seed, double fuzz);

#endif