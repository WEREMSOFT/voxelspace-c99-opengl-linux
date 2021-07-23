#include "camera.h"

Camera cameraCreate(double aspectRatio, Point3 origin)
{
    Camera this = {0};
    this.origin = origin;
    this.aspectRatio = aspectRatio;
    this.viewPortHeight = -2.0;
    this.viewPortWidth = aspectRatio * this.viewPortHeight;
    this.focalLength = 1.0;
    this.horizontal = vec3Create(this.viewPortWidth, 0, 0);
    this.vertical = vec3Create(0, this.viewPortHeight, 0);
    {
        Vec3 temp = vec3Mult(this.horizontal, .5);
        Vec3 point1 = vec3Substract(this.origin, temp);
        temp = vec3Mult(this.vertical, .5);
        Vec3 point2 = vec3Substract(point1, temp);
        temp = vec3Create(0, 0, this.focalLength);
        this.lowerLeft = vec3Substract(point2, temp);
    }
    return this;
}