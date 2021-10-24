#pragma once

#include "BoundingBox.h"
#include "Error.h"
#include "Trace.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <utility>

struct TraceFactoryImpl;

struct TraceFactory
{
    static std::pair<std::shared_ptr<TraceFactory>, Error> getInstance(float windowHeightOverWidth);

    std::pair<std::shared_ptr<Trace>, Error> make(BoundingBox const& allowedBox);
    std::pair<std::shared_ptr<Trace>, Error> make(glm::vec2 const& initialPosition, float initialDirection_);
    static void setNormalCoordinatesTransform(float windowHeightOverWidth);

private:
    static std::pair<std::shared_ptr<TraceFactoryImpl>, Error> createInstance(float windowHeightOverWidth);

    static std::shared_ptr<TraceFactoryImpl> pImpl;
    static Error instanceCreationError;
};
