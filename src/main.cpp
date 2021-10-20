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

glm::vec2 getMouseCursorPosition(GLFWwindow* window)
{
    glm::dvec2 mousePosition;
    glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
    std::cout << mousePosition.x << "," << mousePosition.y << std::endl;
    return glm::vec2{mousePosition};
}

glm::vec2 getWindowSize(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return glm::vec2{width, height};
}

glm::vec2 toNormalizedCoordinates(GLFWwindow* window, glm::vec2 const& pixelCoordinates)
{
    glm::vec2 windowSize = getWindowSize(window);
    glm::vec2 v = (2.0f * glm::vec2{pixelCoordinates.x / windowSize.x,
                             pixelCoordinates.y / windowSize.y}) -
           glm::vec2{1.0, 1.0};
    return glm::vec2{v.x, -v.y};
}

void mouseBtnFun(GLFWwindow* window, int button, int action, int mod)
{
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
    {
        glm::vec2 mousePosition = getMouseCursorPosition(window);
        glm::vec2 normalizedMousePosition = toNormalizedCoordinates(window, mousePosition);

        std::cout << "mouse: (" << normalizedMousePosition.x << "," << normalizedMousePosition.y << ")" << std::endl;

        std::vector<std::shared_ptr<Trace>> *vpTraces = reinterpret_cast<std::vector<std::shared_ptr<Trace>> *>(glfwGetWindowUserPointer(window));
        auto [pTrace, err] = TraceFactory::make(normalizedMousePosition, uniformInInterval(0, 2 * M_PI));
        if (err == nil)
        {
            vpTraces->push_back(pTrace);
            return;
        }
        std::cout << "could not create trace at mouse position: " << err.value() << std::endl;
    }

    if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
    {
        std::vector<std::shared_ptr<Trace>> *vpTraces = reinterpret_cast<std::vector<std::shared_ptr<Trace>> *>(glfwGetWindowUserPointer(window));
        vpTraces->clear();
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

    constexpr std::size_t kMaxTraces{200};

    w.select();
    w.show();

    glm::vec2 const topLeft{-1.0, 1.0};
    glm::vec2 const bottomRight{1.0, -1.0};
    std::vector<std::shared_ptr<Trace>> vpTraces = genTraces(10, topLeft, bottomRight);
    auto [pDoubleFramebuffer, err] = DoubleFramebuffer::get(kWidth, kHeight);
    if (err != nil)
    {
        std::cout << "could not create double framebuffer:" << err.value() << std::endl;
        return -2;
    }

    glfwSetWindowUserPointer(w.pWindow_, &vpTraces);
    glfwSetKeyCallback(w.pWindow_, keyFunc);
    glfwSetMouseButtonCallback(w.pWindow_, mouseBtnFun);

    std::size_t prevTraceCount = vpTraces.size();

    std::optional<std::chrono::steady_clock::time_point> then;
    std::size_t missedDeadlines = 0;
    std::size_t totalIterations = 0;

    while (w && !w.shouldClose())
    {
        ++totalIterations;
        auto now = std::chrono::steady_clock::now();
        if (then)
        {
            auto overTime = (now - then.value()).count() / 1'000'000 - periodMs().count();
            if (overTime > 0 && ++missedDeadlines % 50 == 0)
                std::cout << "MISSED DEADLINES "
                          << 100.0 * static_cast<double>(missedDeadlines) / totalIterations << "%" << std::endl;
        }
        then = now;
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
            if (!BoundingBox{topLeft, bottomRight}.contains(pTrace->position_) ||
            (vpTraces.size() > (kMaxTraces - (kMaxTraces / 5))))
            {
                std::cout << "KILL " << *pTrace << std::endl;
                pTrace->kill();
                itTrace = vpTraces.erase(itTrace);
                tracesChanged = true;
                continue;
            }
            if (pTrace->isDead())
            {
                //std::cout << "DEAD " << *pTrace << std::endl;
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
            pTrace->step(periodMs());
            itTrace++;
        }
        for (auto pTrace : newTraces)
        {
            //std::cout << "NEW " << *pTrace << std::endl;
            vpTraces.push_back(pTrace);
        }
        if (tracesChanged)
        {
            //std::cout << "count " << prevTraceCount << " -> " << vpTraces.size() << std::endl;
            //std::cout << std::endl << "============================" << std::endl << std::endl;
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
