#pragma once

#include "BoundingBox.h"
#include "Error.h"
#include "Trace.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <utility>

struct TraceFactory
{
    static std::pair<std::shared_ptr<Trace>, Error> make(BoundingBox const& allowedBox);

    static std::shared_ptr<const GLuint> pProgram;
    static std::shared_ptr<const GLuint> pBuffer;
    static Error programCreationError;
};
