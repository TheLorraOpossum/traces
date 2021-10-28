#pragma once

#include "BulkRenderer.h"
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

    std::pair<std::shared_ptr<Trace>, Error> make(BoundingBox const& allowedBox, const glm::vec3 &color, std::chrono::steady_clock::time_point const& creationTime);
    std::pair<std::shared_ptr<Trace>, Error> make(glm::vec2 const& initialPosition, float initialDirection_, glm::vec3 const& color, std::chrono::steady_clock::time_point const& creationTime);
    static void setNormalCoordinatesTransform(float windowHeightOverWidth);

    std::shared_ptr<BulkRenderer> getBulkRenderer() const;

private:
    static std::pair<std::shared_ptr<TraceFactoryImpl>, Error> createInstance(float windowHeightOverWidth);

    static std::shared_ptr<TraceFactoryImpl> pImpl;
    static Error instanceCreationError;
};
