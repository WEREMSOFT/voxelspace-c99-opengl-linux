#include "material.h"
#include "../ray/ray.h"
#include "../hittable/hittable.h"
#include "../utils/utils.h"

bool lambertianMaterialScatter(Vec3 albedo, Ray ray, HitRecord *hitRecord, Vec3 *pAtenuation, Ray *pScattered, unsigned int *seed)
{

    Vec3 scatterDirection = vec3Add(hitRecord->point, randomUnitVector(seed));

    if (vec3IsNearZero(scatterDirection))
        scatterDirection = hitRecord->normal;

    Ray scatteredRay = rayCreate(hitRecord->point, scatterDirection);
    pScattered->direction = scatteredRay.direction;
    pScattered->origin = scatteredRay.origin;
    pAtenuation->x = albedo.x;
    pAtenuation->y = albedo.y;
    pAtenuation->z = albedo.z;

    return true;
}

bool metalMaterialScatter(Vec3 albedo, Ray pRay, HitRecord *hitRecord, Vec3 *atenuation, Ray *pScattered, unsigned int *seed, double fuzz)
{

    Vec3 reflected = vec3Reflect(vec3Unit(pRay.direction), hitRecord->normal);

    Ray scattered = rayCreate(hitRecord->point, vec3Add(reflected, vec3Mult(randomInUnitSphere(seed), fuzz)));
    *pScattered = scattered;

    atenuation->x = albedo.x;
    atenuation->y = albedo.y;
    atenuation->z = albedo.z;

    return vec3Dot(pScattered->direction, hitRecord->normal) > 0;
}
