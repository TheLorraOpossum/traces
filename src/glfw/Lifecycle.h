#pragma once

#include <GLFW/glfw3.h>

namespace glfw {
struct Lifecycle
{
    Lifecycle()
    {
        glfwInit();
    }
    ~Lifecycle()
    {
        glfwTerminate();
    }
};
} // namespace glfw