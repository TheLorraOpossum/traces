#pragma once

#include "Error.h"
#include "Trace.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <utility>

struct TraceFactory
{
    static std::pair<std::shared_ptr<Trace>, Error> make(glm::vec2 const& boundTopLeft, glm::vec2 const& boundBottomRight);

    static std::shared_ptr<const GLuint> pProgram;
    static std::shared_ptr<const GLuint> pBuffer;
    static Error programCreationError;
};
