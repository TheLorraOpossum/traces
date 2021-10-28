#include "TraceFactory.h"
#include "Utils.h"
#include "Program.h"
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

struct TraceFactoryImpl
{
    std::pair<std::shared_ptr<Trace>, Error> make(BoundingBox const& allowedBox, glm::vec3 const& color, const std::chrono::steady_clock::time_point &creationTime);
    std::pair<std::shared_ptr<Trace>, Error> make(glm::vec2 const& initialPosition, float initialDirection_, glm::vec3 const& color, const std::chrono::steady_clock::time_point &creationTime);
    void setNormalCoordinatesTransform(float windowHeightOverWidth);

    std::shared_ptr<const GLuint> pProgram;
    std::shared_ptr<const GLuint> pBuffer;
};


std::pair<std::shared_ptr<const GLuint>, Error> makeProgram()
{
    auto [pVert, pFrag, err] = loadShaderPair("../src/shaders", "trace");
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not load shaders:", err.value()));
    }
    return makeProgram(pVert, pFrag);
}

std::pair<std::shared_ptr<Trace>, Error> TraceFactoryImpl::make(BoundingBox const &allowedBox, glm::vec3 const& color, const std::chrono::steady_clock::time_point &creationTime)
{
    glm::vec2 initialPosition = uniformInBox(allowedBox);
    return std::make_pair(std::make_shared<Trace>(initialPosition, uniformInInterval(0, 2 * M_PI), color, creationTime, pProgram, pBuffer), nil);
}

std::pair<std::shared_ptr<Trace>, Error> TraceFactoryImpl::make(glm::vec2 const &initialPosition, float initialDirection_, glm::vec3 const& color, const std::chrono::steady_clock::time_point &creationTime)
{
    return std::make_pair(std::make_shared<Trace>(initialPosition, initialDirection_, color, creationTime, pProgram, pBuffer), nil);
}

void TraceFactoryImpl::setNormalCoordinatesTransform(float windowHeightOverWidth)
{
    glUseProgram(*pProgram);
    glm::mat2 toNormalCoordinates{
        glm::vec2{windowHeightOverWidth, 0.0f},
        glm::vec2{0.0f, 1.0f}};
    glUniformMatrix2fv(glGetUniformLocation(*pProgram, "toNormalCoordinates"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(toNormalCoordinates));
    glUseProgram(InvalidId);
}

std::pair<std::shared_ptr<TraceFactory>, Error> TraceFactory::getInstance(float windowHeightOverWidth)
{
    if (!pImpl && instanceCreationError) return std::make_pair(nullptr, makeError("could not make instance:", instanceCreationError.value()));
    if (!instanceCreationError)
    {
        std::tie(pImpl, instanceCreationError) = createInstance(windowHeightOverWidth);
        if (instanceCreationError) return std::make_pair(nullptr, makeError("could not make instance:", instanceCreationError.value()));
    }
    return std::make_pair(std::make_shared<TraceFactory>(), nil);
}

std::pair<std::shared_ptr<Trace>, Error> TraceFactory::make(BoundingBox const& allowedBox, glm::vec3 const& color, const std::chrono::steady_clock::time_point &creationTime)
{
    if (!pImpl) return std::make_pair(nullptr, makeError("no TraceFactory instance:", instanceCreationError.value()));
    return pImpl->make(allowedBox, color, creationTime);
}

std::pair<std::shared_ptr<Trace>, Error> TraceFactory::make(glm::vec2 const &initialPosition, float initialDirection_, glm::vec3 const& color, const std::chrono::steady_clock::time_point &creationTime)
{
    if (!pImpl) return std::make_pair(nullptr, makeError("no TraceFactory instance:", instanceCreationError.value()));
    return pImpl->make(initialPosition, initialDirection_, color, creationTime);
}

void TraceFactory::setNormalCoordinatesTransform(float windowHeightOverWidth)
{
    if (!pImpl) return;
    pImpl->setNormalCoordinatesTransform(windowHeightOverWidth);
}

std::shared_ptr<BulkRenderer> TraceFactory::getBulkRenderer() const
{
    if (!pImpl) return nullptr;
    return std::make_shared<BulkRenderer>(*pImpl->pProgram, *pImpl->pBuffer);
}

std::pair<std::shared_ptr<TraceFactoryImpl>, Error> TraceFactory::createInstance(float windowHeightOverWidth)
{
    if (!pImpl)
    {
        if (instanceCreationError)
        {
            return std::make_pair(nullptr, makeError("could not create TraceFactory instance, don't have program:", instanceCreationError.value()));
        }
        auto [pProgram, err] = makeProgram();
        if (err != nil)
        {
            instanceCreationError = err;
            return std::make_pair(nullptr, makeError("could not create TraceFactory instance, failed building program:", instanceCreationError.value()));
        }
        pImpl = std::make_shared<TraceFactoryImpl>();
        pImpl->pProgram = pProgram;
        pImpl->setNormalCoordinatesTransform(windowHeightOverWidth);
    }
    if (!pImpl->pBuffer)
        pImpl->pBuffer = genBuffer();

    return std::make_pair(pImpl, nil);
}

std::shared_ptr<TraceFactoryImpl> TraceFactory::pImpl;
Error TraceFactory::instanceCreationError;
