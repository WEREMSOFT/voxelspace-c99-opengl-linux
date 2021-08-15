#include "utils.h"

void printFPS(Graphics this, double deltaTime)
{
#define FPS_HISTORY 10
    static double fpsHistory[FPS_HISTORY] = {0};
    static int counter = 1;
    counter++;
    counter %= FPS_HISTORY;
    fpsHistory[counter] = (1 / deltaTime);

    int sum = 0;
    for (int i = 0; i < FPS_HISTORY; i++)
    {
        sum += fpsHistory[i];
    }

    float avg = sum / FPS_HISTORY;

    {
        char text[1000] = {0};
        snprintf(text, 1000, "fps: %d", (int)floor(avg));
        imPrintString(this.imageData, (PointI){100, 0}, text, (Color){0, 0xff, 0xff});
    }
}

double getDeltaTime()
{
    static double lastUpdate = 0;

    double deltaTime = glfwGetTime() - lastUpdate;
    lastUpdate = glfwGetTime();

    return deltaTime;
}

#define RANDOM_TABLE_SIZE 1000

static double randomTable[RANDOM_TABLE_SIZE] = {0};

void initRandomTable(void)
{
    for (int i = 0; i < RANDOM_TABLE_SIZE; i++)
    {
        randomTable[i] = random() / (RAND_MAX + 1.0);
    }
}

double randomDouble(unsigned int *seed)
{
    return randomTable[rand_r(seed) % RANDOM_TABLE_SIZE];
}

double randomDoubleMinMax(double min, double max, unsigned int *seed)
{
    return min + (max - min) * randomDouble(seed);
}

double clamp(double pValue, double pMin, double pMax)
{
    return fmaxf(pMin, fminf(pValue, pMax));
}

Vec3 randomInUnitSphere(unsigned int *seed)
{
    while (1)
    {
        Vec3 point = vec3RandomMinMax(-1, 1, seed);
        if (vec3LengthSqurd(point) <= 1)
        {
            return point;
        }
    }
}

Vec3 randomUnitVector(unsigned int *seed)
{
    return vec3Unit(randomInUnitSphere(seed));
}

Vec3 randomInHemisphere(Vec3 pNormal, unsigned int *seed)
{
    Vec3 inUnitSphere = randomInUnitSphere(seed);
    if (vec3Dot(inUnitSphere, pNormal) > 0.0)
        return inUnitSphere;
    else
        return vec3Mult(inUnitSphere, -1.0);
}