#include "program.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define _GNU_SOURCE
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "core/input/input.h"
#include "utils/utils.h"
#include "sphere/sphere.h"
#include "vec3/vec3.h"

#define PTHREAD_COUNT 8

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#ifdef ARRAY_MALLOC
#undef ARRAY_MALLOC
#endif

#ifdef ARRAY_REALLOC
#undef ARRAY_REALLOC
#endif

#define __UNIVERSAL_ARRAY_IMPLEMENTATION__
#include "core/array/array.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void imRenderVoxelSpace(Program this, PointI pPoint);

Program programCreate()
{
    Program this = {0};
    this.graphics = graphicsCreate(640, 480);

    this.sound = soundCreate();
    Soloud_setGlobalVolume(this.sound.soloud, 0);

    this.heightMap = spriteCreate("assets/height.png");
    this.heightMap.position.x = this.heightMap.imageData.size.x;
    this.colorMap = spriteCreate("assets/color.png");

    this.height = 50;
    this.horizon = 120;
    this.scale.y = 300;
    this.distance = 1000;
    this.cameraSpeed = 5.0;
    return this;
}

void programMainLoop(Program this)
{
    PointI cameraPosition = {0};
    double deltaTime = getDeltaTime();
    while (glfwGetKey(this.graphics.window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        if (glfwGetKey(this.graphics.window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cameraPosition.x += this.cameraSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cameraPosition.x -= this.cameraSpeed * deltaTime;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_UP) == GLFW_PRESS)
            cameraPosition.y -= this.cameraSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_DOWN) == GLFW_PRESS)
            cameraPosition.y += this.cameraSpeed * deltaTime;

        if (glfwGetKey(this.graphics.window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
            this.height += this.cameraSpeed * deltaTime;
        else if (glfwGetKey(this.graphics.window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
            this.height -= this.cameraSpeed * deltaTime;

        imClear(this.graphics.imageData);
        imRenderVoxelSpace(this, cameraPosition);

        printFPS(this.graphics, getDeltaTime());

        graphicsSwapBuffers(this.graphics);
        glfwPollEvents();
    }
}

void programDestroy(Program this)
{
    spriteDestroy(this.heightMap);
    spriteDestroy(this.colorMap);
    graphicsDestroy(this.graphics);
    soundDestroy(this.sound);
}

void drawVerticalLine(ImageData this, PointI start, Color color, int maxHeight)
{
    for (int i = start.y; i < maxHeight; i++)
    {
        start.y = i;
        imPutPixel(this, start, color);
    }
}

void imRenderVoxelSpace(Program this, PointI pPoint)
{
    PointF pLeft = {0};
    PointF pRight = {0};
    int maxHeight[this.graphics.imageData.size.x];

    for (int i = 0; i < this.graphics.imageData.size.x; i++)
    {
        maxHeight[i] = this.graphics.imageData.size.y - 1;
    }

    for (int z = 1; z < this.distance; z++)
    {
        pLeft.y = pRight.y = -z + pPoint.y;
        pRight.x = -z + pPoint.x;
        pLeft.x = z + pPoint.x;

        double dx = (pRight.x - pLeft.x) / this.graphics.imageData.size.x;

        for (int i = 0; i < this.graphics.imageData.size.x; i++)
        {
            PointU mappedPoint = pointFToPointU(pLeft);

            unsigned int position = (mappedPoint.x + mappedPoint.y * this.heightMap.imageData.size.x) % this.heightMap.imageData.bufferSize;
            Color heightMapColor = this.heightMap.imageData.data[position];
            Color colorMapColor = this.colorMap.imageData.data[position];
            int heightOnScreen = (this.height - heightMapColor.r) / (float)z * this.scale.y + this.horizon;

            heightOnScreen = MIN(MAX(heightOnScreen, 0), this.graphics.imageData.size.y);
            drawVerticalLine(this.graphics.imageData, (PointI){i, heightOnScreen}, colorMapColor, maxHeight[i]);
            pLeft.x += dx;
            maxHeight[i] = MIN(heightOnScreen, maxHeight[i]);
        }
    }
}

/*
def Render(p, height, horizon, scale_height, distance, screen_width, screen_height):
    # Draw from back to the front (high z coordinate to low z coordinate)
    for z in range(distance, 1, -1):
        # Find line on map. This calculation corresponds to a field of view of 90°
        pleft  = Point(-z + p.x, -z + p.y)
        pright = Point( z + p.x, -z + p.y)
        # segment the line
        dx = (pright.x - pleft.x) / screen_width
        # Raster line and draw a vertical line for each segment
        for i in range(0, screen_width):
            height_on_screen = (height - heightmap[pleft.x, pleft.y]) / z * scale_height. + horizon
            DrawVerticalLine(i, height_on_screen, screen_height, colormap[pleft.x, pleft.y])
            pleft.x += dx

# Call the render function with the camera parameters:
# position, height, horizon line position,
# scaling factor for the height, the largest distance, 
# screen width and the screen height parameter
Render( Point(0, 0), 50, 120, 120, 300, 800, 600 )
*/