#include "mouse.h"

static int mouseButtons[3] = {0};

bool isMouseButtonJustPressed(GLFWwindow *window, int mouseButton)
{
    int currentMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);

    if (mouseButtons[mouseButton] && currentMouseState)
        return false;

    mouseButtons[mouseButton] = currentMouseState;

    return currentMouseState;
}