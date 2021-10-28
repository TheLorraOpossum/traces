#include "Utils.h"
#include "Trace.h"
#include "TraceFactory.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <random>

namespace {
std::random_device rd;
std::mt19937 gen{rd()};
}

std::pair<std::string, Error> readFile(std::string const& path)
{
    std::ifstream f;
    f.open(path, std::ios::in);
    if (!f.is_open())
    {
        return std::make_pair(std::string{""}, makeError("could not open file:", path));
    }
    std::ostringstream stream;
    stream << f.rdbuf();

    return std::make_pair(stream.str(), nil);
}

glm::vec2 uniformInBox(glm::vec2 const& boundTopLeft, glm::vec2 const& boundBottomRight)
{
    std::uniform_real_distribution<float> unifX{boundTopLeft.x, boundBottomRight.x};
    std::uniform_real_distribution<float> unifY{boundBottomRight.y, boundTopLeft.y};
    return glm::vec2{unifX(gen), unifY(gen)};
}

glm::vec2 uniformInBox(BoundingBox const& bb)
{
    return uniformInBox(bb.topLeft, bb.bottomRight);
}

float uniformInInterval(float left, float right)
{
    std::uniform_real_distribution<float> dis{left, right};
    return dis(gen);
}

std::shared_ptr<GLuint const> genBuffer()
{
    GLuint buffer{InvalidId};
    glGenBuffers(1, &buffer);

    auto lDeleter = [](GLuint *pId)
    {
        if (!pId)
            return;
        glDeleteBuffers(1, pId);
    };

    std::shared_ptr<GLuint> pBufferLocal;

    pBufferLocal.reset(new (std::nothrow) GLuint(buffer), lDeleter);
    return pBufferLocal;
}


glm::vec2 getMouseCursorPosition(GLFWwindow *window)
{
    glm::dvec2 mousePosition;
    glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
    //std::cout << mousePosition.x << "," << mousePosition.y << std::endl;
    return glm::vec2{mousePosition};
}

glm::vec2 getWindowSize(GLFWwindow *window)
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

glm::vec2 toNormalizedCoordinates(GLFWwindow *window, const glm::vec2 &pixelCoordinates)
{
    glm::vec2 windowSize = getWindowSize(window);
    glm::vec2 v = (2.0f * glm::vec2{pixelCoordinates.x / windowSize.x,
                                    pixelCoordinates.y / windowSize.y}) -
            glm::vec2{1.0, 1.0};
    return glm::vec2{v.x, -v.y};
}

void mouseBtnFun(GLFWwindow *window, int button, int action, int mod)
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
        auto [pTrace, err1] = pTraceFactory->make(monometricPosition,
                uniformInInterval(0, 2 * M_PI),
                glm::vec3{uniformInInterval(0, 1),uniformInInterval(0, 1),uniformInInterval(0, 1)},
                now);
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

std::vector<std::shared_ptr<Trace> > genTraces(int count, const glm::vec2 &topLeft, const glm::vec2 &bottomRight, glm::vec3 const& color, std::shared_ptr<TraceFactory> pTraceFactory)
{
    auto now = std::chrono::steady_clock::now();
    std::vector<std::shared_ptr<Trace>> vpTraces;
    for (int i = 0; i < count; i++)
    {
        auto [pTrace, err] = pTraceFactory->make(BoundingBox{glm::vec2{-1.0, 1.0}, glm::vec2{1.0, -1.0}}, color, now);
        if (err != nil)
        {
            std::cout << "could not make trace: " << err.value() << std::endl;
            continue;
        }
        vpTraces.push_back(pTrace);
    }

    return vpTraces;
}
