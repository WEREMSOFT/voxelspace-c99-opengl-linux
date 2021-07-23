#ifndef __UTILS_H__
#define __UTILS_H__
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../core/graphics/graphics.h"
#include "../vec3/vec3.h"

void initRandomTable(void);
double randomDouble(unsigned int *seed);
double randomDoubleMinMax(double min, double max, unsigned int *seed);
double clamp(double pValue, double pMin, double pMax);
Vec3 randomInUnitSphere(unsigned int *seed);
Vec3 randomUnitVector(unsigned int *seed);
Vec3 randomInHemisphere(Vec3 pNormal, unsigned int *seed);
void printFPS(Graphics this, double deltaTime);
double getDeltaTime();
#endif