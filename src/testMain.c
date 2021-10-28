#include <stdio.h>
#include <time.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "traces_render.h"

static struct TracesScenarioOptions getOptions(int width, int height);

int main()
{
    glfwInit();
    GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
    GLFWvidmode const * pVideoMode = glfwGetVideoMode(pMonitor);
    glfwWindowHint(GLFW_SAMPLES, 4);
    int const width = pVideoMode->width;
    int const height = pVideoMode->height;
    GLFWwindow* w = glfwCreateWindow(width, height, "WIN", pMonitor, NULL);
    glfwMakeContextCurrent(w);
    glewInit();
    glfwSwapInterval(1);

    struct TracesScenarioOptions options = getOptions(width, height);
    ScenarioHandle h = newScenario(options);

    int fps = 0;
    struct timespec lastFpsPrint;
    clock_gettime(CLOCK_MONOTONIC, &lastFpsPrint);

    while (!glfwWindowShouldClose(w))
    {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        int deltaTSec = now.tv_sec - lastFpsPrint.tv_sec;
        if (deltaTSec >= 1)
        {
            printf("FPS: %5.2f\n", (float)fps / deltaTSec);
            lastFpsPrint = now;
            fps = 0;
        }

        glfwPollEvents();

        stepScenario(h);
        drawScenario(h);

        fps += 1;
        glfwSwapBuffers(w);
    }

    releaseScenario(h);
    glfwDestroyWindow(w);
    glfwTerminate();

    return 0;
}

static struct TracesScenarioOptions getOptions(int width, int height)
{
    struct TracesScenarioOptions options;

    options.colorR = 0.0f;
    options.colorG = 1.0f;
    options.colorB = 0.0f;

    options.width = width;
    options.height = height;

    options.initialTraceCount = 20;
    options.maxTraces = 200;

    options.splitProbability = 0.4f;
    options.stepPeriodMs = 16;

    return options;
}
