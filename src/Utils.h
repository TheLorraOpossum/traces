#pragma once

#include "BoundingBox.h"
#include "Error.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct GLFWwindow;
struct Trace;
struct TraceFactory;

std::pair<std::string, Error> readFile(std::string const& path);
glm::vec2 uniformInBox(glm::vec2 const& boundTopLeft, glm::vec2 const& boundBottomRight);
glm::vec2 uniformInBox(BoundingBox const& bb);
float uniformInInterval(float left, float right);
std::shared_ptr<GLuint const> genBuffer();

glm::vec2 getMouseCursorPosition(GLFWwindow* window);

glm::vec2 getWindowSize(GLFWwindow* window);

std::pair<glm::ivec2, Error> getMonitorCurrentVideoMode();

glm::vec2 toNormalizedCoordinates(GLFWwindow* window, glm::vec2 const& pixelCoordinates);

void mouseBtnFun(GLFWwindow* window, int button, int action, int mod);

std::vector<std::shared_ptr<Trace>> genTraces(int count, glm::vec2 const& topLeft, glm::vec2 const& bottomRight, const glm::vec3 &color, std::shared_ptr<TraceFactory> pTraceFactory);

inline std::chrono::milliseconds periodMs()
{
    return std::chrono::milliseconds{16};
}
constexpr GLuint InvalidId = 0u;
