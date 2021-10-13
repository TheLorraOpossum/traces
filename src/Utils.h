#pragma once

#include "Error.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <utility>

std::pair<std::string, Error> readFile(std::string const& path);
glm::vec2 uniformInBox(glm::vec2 const& boundTopLeft, glm::vec2 const& boundBottomRight);
std::shared_ptr<GLuint const> genBuffer();

constexpr GLuint InvalidId = 0u;