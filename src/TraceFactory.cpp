#include "TraceFactory.h"
#include "Utils.h"
#include "Program.h"
#include "Shader.h"

std::pair<std::shared_ptr<const GLuint>, Error> makeProgram()
{
    auto [pVert, pFrag, err] = loadShaderPair("../src/shaders", "trace");
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not load shaders:", err.value()));
    }
    return makeProgram(pVert, pFrag);
}

std::pair<std::shared_ptr<Trace>, Error> TraceFactory::make(glm::vec2 const &boundTopLeft, glm::vec2 const &boundBottomRight)
{
    if (!pProgram)
    {
        if (programCreationError)
        {
            return std::make_pair(nullptr, makeError("could not create trace, don't have program:", programCreationError.value()));
        }
        std::tie(pProgram, programCreationError) = makeProgram();
        if (programCreationError)
        {
            return std::make_pair(nullptr, makeError("could not create trace, failed building program:", programCreationError.value()));
        }
    }
    if (!pBuffer)
        pBuffer = genBuffer();

    glm::vec2 initialPosition = uniformInBox(boundTopLeft, boundBottomRight);
    return std::make_pair(std::make_shared<Trace>(initialPosition, pProgram, pBuffer), nil);
}

std::shared_ptr<const GLuint> TraceFactory::pProgram;
std::shared_ptr<const GLuint> TraceFactory::pBuffer;
Error TraceFactory::programCreationError{nil};