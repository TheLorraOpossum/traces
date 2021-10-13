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
        auto [pTrace, err] = TraceFactory::make(topLeft, bottomRight);
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
    constexpr int kWidth = 640;
    constexpr int kHeight = 480;
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

        auto itTrace = vpTraces.begin();
        std::vector<std::shared_ptr<Trace>> newTraces;
        bool anyTracesDead{false};
        while (itTrace != vpTraces.end())
        {
            auto pTrace = *itTrace;
            if (!pTrace->isDead())
            {
                itTrace++;
                continue;
            }
            anyTracesDead = true;
            if (rand() / static_cast<double>(RAND_MAX) > 0.5)
            {
                auto [pTrace1, pTrace2] = pTrace->split();
                newTraces.push_back(pTrace1);
                newTraces.push_back(pTrace2);
                std::cout << "SPLIT\t";
            }
            else
            {
                std::cout << "DIE\t";
            }
            std::cout << *pTrace << std::endl;
            itTrace = vpTraces.erase(itTrace);
        }
        
        for (auto pTrace : newTraces)
        {
            vpTraces.push_back(pTrace);
        }
        if (anyTracesDead)
        {
            std::cout << "\ntrace count " << prevTraceCount << " -> " << vpTraces.size() << std::endl;
            std::cout << "\n=========\n" << std::endl;
        }

        itTrace = vpTraces.begin();
        while (itTrace != vpTraces.end())
        {
            auto pTrace = *itTrace;
            pTrace->step(16ms);
            pTrace->render();
            itTrace++;
        }

        pDoubleFramebuffer->blitAndSwap();
        w.present();
    }

    return 0;
}
