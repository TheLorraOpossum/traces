#pragma once

#include <GL/glew.h>
#include "Error.h"
#include <memory>
#include <string>
#include <utility>
#include <tuple>

std::pair<std::shared_ptr<const GLuint>, Error> loadShader(std::string const& path);
std::tuple<std::shared_ptr<const GLuint>, std::shared_ptr<const GLuint>, Error> loadShaderPair(std::string const& directory, std::string const& name);
std::pair<std::shared_ptr<const GLuint>, Error> makeShader(std::string const& source, GLenum type);
std::tuple<std::shared_ptr<const GLuint>, std::shared_ptr<const GLuint>, Error> makeShaderPair(std::string const& vertexShaderSource, std::string const& fragmentShaderSource);
