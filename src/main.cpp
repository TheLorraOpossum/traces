#include "DoubleFramebuffer.h"
#include "Error.h"
#include "Trace.h"
#include "TraceFactory.h"
#include "Utils.h"
#include "glfw/Lifecycle.h"
#include "glfw/Window.h"


#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>


void keyFunc(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    std::vector<std::shared_ptr<Trace>> *vpTraces = reinterpret_cast<std::vector<std::shared_ptr<Trace>>*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        for (auto pTrace : *vpTraces) pTrace->step(std::chrono::milliseconds{100});
    }
}

std::vector<std::shared_ptr<Trace>> genTraces(int count, glm::vec2 const& topLeft, glm::vec2 const& bottomRight)
{
    std::vector<std::shared_ptr<Trace>> vpTraces;

    for (int i = 0; i < count; i++)
    {
        auto [pTrace, err] = TraceFactory::make(BoundingBox{glm::vec2{-1.0, 1.0}, glm::vec2{1.0, -1.0}});
        if (err != nil)
        {
            std::cout << "could not make trace: " << err.value() << std::endl;
            continue;
        }
        vpTraces.push_back(pTrace);
    }

    return vpTraces;
}


using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    glfw::Lifecycle lc;
    constexpr int kWidth = 1000;
    constexpr int kHeight = 1000;
    glfw::Window w{kWidth, kHeight, true};

    w.select();
    w.show();

    glm::vec2 const topLeft{-1.0, 1.0};
    glm::vec2 const bottomRight{1.0, -1.0};
    std::vector<std::shared_ptr<Trace>> vpTraces = genTraces(5, topLeft, bottomRight);
    auto [pDoubleFramebuffer, err] = DoubleFramebuffer::get(kWidth, kHeight);
    if (err != nil)
    {
        std::cout << "could not create double framebuffer:" << err.value() << std::endl;
        return -2;
    }

    glfwSetWindowUserPointer(w.pWindow_, &vpTraces);
    glfwSetKeyCallback(w.pWindow_, keyFunc);

    std::size_t prevTraceCount = vpTraces.size();

    while (w && !w.shouldClose())
    {
        w.pollEvents();

        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        pDoubleFramebuffer->renderPreviousFrame();

        std::vector<std::shared_ptr<Trace>> newTraces;
        prevTraceCount = vpTraces.size();
        bool tracesChanged{false};

        auto itTrace = vpTraces.begin();
        while (itTrace != vpTraces.end())
        {
            auto pTrace = *itTrace;
            if (!BoundingBox{topLeft, bottomRight}.contains(pTrace->position_))
            {
                std::cout << "KILL " << *pTrace << std::endl;
                pTrace->kill();
                itTrace = vpTraces.erase(itTrace);
                tracesChanged = true;
                continue;
            }
            if (pTrace->isDead())
            {
                std::cout << "DEAD " << *pTrace << std::endl;
                if (uniformInInterval(0.0, 1.0) > 0.4)
                {
                    auto [pTrace1, pTrace2] = pTrace->split();
                    newTraces.push_back(pTrace1);
                    newTraces.push_back(pTrace2);
                }
                itTrace = vpTraces.erase(itTrace);
                tracesChanged = true;
                continue;
            }
            pTrace->step(16ms);
            itTrace++;
        }
        for (auto pTrace : newTraces)
        {
            std::cout << "NEW " << *pTrace << std::endl;
            vpTraces.push_back(pTrace);
        }
        if (tracesChanged)
        {
            std::cout << "count " << prevTraceCount << " -> " << vpTraces.size() << std::endl;
            std::cout << std::endl << "============================" << std::endl << std::endl;
        }
        for (auto pTrace : vpTraces)
        {
            pTrace->render();
        }

        pDoubleFramebuffer->blitAndSwap();
        w.present();
    }

    return 0;
}
