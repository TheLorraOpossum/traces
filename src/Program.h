#pragma once

#include "Error.h"
#include <GL/glew.h>
#include <memory>
#include <utility>

std::pair<std::shared_ptr<const GLuint>, Error> makeProgram(std::shared_ptr<const GLuint> pVert, std::shared_ptr<const GLuint> pFrag);
