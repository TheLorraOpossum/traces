#include "BulkRenderer.h"
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
#include <iomanip>
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
    //std::cout << mousePosition.x << "," << mousePosition.y << std::endl;
    return glm::vec2{mousePosition};
}

glm::vec2 getWindowSize(GLFWwindow* window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return glm::vec2{width, height};
}

std::pair<glm::ivec2, Error> getMonitorCurrentVideoMode()
{
    auto pMonitor{glfwGetPrimaryMonitor()};
    if (!pMonitor) return std::make_pair(glm::ivec2{}, makeError("no primary monitor"));
    auto pVideoMode{glfwGetVideoMode(pMonitor)};
    if (!pVideoMode) return std::make_pair(glm::ivec2{}, makeError("no video mode"));
    return std::make_pair(glm::ivec2{pVideoMode->width, pVideoMode->height}, nil);
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
        glm::vec2 mousePixelPosition = getMouseCursorPosition(window);

        glm::vec2 windowSize = getWindowSize(window);
        float h = windowSize.y;
        glm::mat2 toMonometric{
            glm::vec2{2.0f/h, 0.0f},
            glm::vec2{0.0f, -2.0f/h}
        };
        glm::vec2 pixelOriginInMonometric{-windowSize.x/windowSize.y, 1.0f};
        glm::vec2 monometricPosition = toMonometric * mousePixelPosition + pixelOriginInMonometric;

        //std::cout << "mouse: (" << monometricPosition.x << "," << monometricPosition.y << ")" << std::endl;

        std::vector<std::shared_ptr<Trace>> *vpTraces = reinterpret_cast<std::vector<std::shared_ptr<Trace>> *>(glfwGetWindowUserPointer(window));
        auto [pTraceFactory, err] = TraceFactory::getInstance(windowSize.y/windowSize.x);
        if (err != nil)
        {
            std::cout << "could not get TraceFactory instance: " << err.value() << std::endl;
            return;
        }
        auto now = std::chrono::steady_clock::now();
        auto [pTrace, err1] = pTraceFactory->make(monometricPosition, uniformInInterval(0, 2 * M_PI), now);
        if (err1 == nil)
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

std::vector<std::shared_ptr<Trace>> genTraces(int count, glm::vec2 const& topLeft, glm::vec2 const& bottomRight, std::shared_ptr<TraceFactory> pTraceFactory)
{
    auto now = std::chrono::steady_clock::now();
    std::vector<std::shared_ptr<Trace>> vpTraces;
    for (int i = 0; i < count; i++)
    {
        auto [pTrace, err] = pTraceFactory->make(BoundingBox{glm::vec2{-1.0, 1.0}, glm::vec2{1.0, -1.0}}, now);
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
//    constexpr int kWidth = 1000;
//    constexpr int kHeight = 1000;
    Error err;
    glm::ivec2 videoMode;
    std::tie(videoMode, err) = getMonitorCurrentVideoMode();
    if (err != nil)
    {
        std::cout << "could not get current monitor video mode:" << err.value();
        return -2;
    }

    glfw::Window w{videoMode.x, videoMode.y, true};
    w.setFullscreen(true);
    float widthOverHeight = static_cast<float>(videoMode.x) / videoMode.y;
    float heightOverWidth = 1.0f/widthOverHeight;

    constexpr std::size_t kMaxTraces{200};

    w.select();
    w.show();

    std::shared_ptr<TraceFactory> pTraceFactory;
    std::tie(pTraceFactory, err) = TraceFactory::getInstance(heightOverWidth);
    if (err != nil)
    {
        std::cout << "could not genTraces():" << err.value() << std::endl;
        return -2;
    }
    glm::vec2 const topLeft{-widthOverHeight, 1.0};
    glm::vec2 const bottomRight{widthOverHeight, -1.0};
    std::vector<std::shared_ptr<Trace>> vpTraces = genTraces(20, topLeft, bottomRight, pTraceFactory);
    std::shared_ptr<DoubleFramebuffer> pDoubleFramebuffer;
    std::tie(pDoubleFramebuffer, err) = DoubleFramebuffer::get(videoMode.x, videoMode.y);
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

    bool missedDeadlinesPrinted{false};
    int partialIterations{0};
    std::optional<std::chrono::steady_clock::time_point> lastFpsPrint;

    while (w && !w.shouldClose())
    {
        ++totalIterations;
        ++partialIterations;
        auto now = std::chrono::steady_clock::now();
        if (then)
        {
            auto overTime = (now - then.value()).count() / 1'000'000 - periodMs().count();
            if (overTime > 0)
            {
                missedDeadlinesPrinted = false;
                missedDeadlines++;
            }
            if (missedDeadlines % 50 == 0)
            {
                missedDeadlinesPrinted = true;
                std::cout << now.time_since_epoch().count() / 1'000'000 << " MISSED DEADLINES " //<< std::setw(5) << std::setprecision(5)
                          << 100.0 * static_cast<double>(missedDeadlines) / totalIterations << "%" << std::endl;
            }
            auto fpsInterval = now - lastFpsPrint.value_or(now);
            if (!lastFpsPrint || fpsInterval >= 1s)
            {
                std::cout << now.time_since_epoch().count() / 1e9 << " " << partialIterations / (fpsInterval.count()/1e9) << "fps" << std::endl;
                partialIterations = 0;
                lastFpsPrint = now;
            }
        }
        then = now;
        w.pollEvents();

        std::vector<std::shared_ptr<Trace>> newTraces;
        prevTraceCount = vpTraces.size();
        bool tracesChanged{false};

        auto itTrace = vpTraces.begin();
        while (itTrace != vpTraces.end())
        {
            auto pTrace = *itTrace;
            bool traceOutOfBoundary = !BoundingBox{topLeft, bottomRight}.contains(pTrace->position_);
            if (traceOutOfBoundary ||
            (vpTraces.size() > (kMaxTraces - (kMaxTraces / 5))))
            {
                //std::cout << "KILL (" << (traceOutOfBoundary ? "B": "L") << ") " << *pTrace << std::endl;
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


        auto pBulkRenderer(pTraceFactory->getBulkRenderer());
        if (pBulkRenderer)
        {
            for (auto pTrace : vpTraces)
            {
                pBulkRenderer->add(pTrace);
            }
            pBulkRenderer->bufferData();
        }

        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        pDoubleFramebuffer->renderPreviousFrame();
        if (pBulkRenderer)
        {
            pBulkRenderer->setColor(glm::vec3{1.0, 0.0, 1.0});
            pBulkRenderer->render();
        }

        pDoubleFramebuffer->blitAndSwap();
        w.present();
    }

    return 0;
}
