#include "BulkRenderer.h"
#include "Trace.h"
#include "Utils.h"
#include <glm/glm.hpp>

BulkRenderer::BulkRenderer(GLuint const& traceProgram, GLuint const& vertexBuffer)
    : program{traceProgram}
    , buffer{vertexBuffer}
{}

void BulkRenderer::add(std::shared_ptr<Trace> pTrace)
{
    vpTraces.push_back(pTrace);
}

void BulkRenderer::bufferData()
{
    glUseProgram(program);
    std::vector<glm::vec2> vs;
    for (auto const pTrace : vpTraces)
    {
        if (!pTrace) continue;
        vs.push_back(pTrace->prevPosition_);
        vs.push_back(pTrace->position_);
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(vs.size()) * sizeof(vs[0]), vs.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, InvalidId);
    glUseProgram(InvalidId);
}

void BulkRenderer::render()
{
    if (program == InvalidId) return;

    glUseProgram(program);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    GLint positionLocation = glGetAttribLocation(program, "positionMonometric");
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(positionLocation);

    glDrawArrays(GL_LINES, 0, vpTraces.size() * sizeof(vpTraces[0]));

    glDisableVertexAttribArray(positionLocation);
    glBindBuffer(GL_ARRAY_BUFFER, InvalidId);
    glUseProgram(InvalidId);

    vpTraces.clear();
}

void BulkRenderer::setColor(glm::vec3 const& color)
{
    if (program == InvalidId) return;
    glUseProgram(program);
    glUniform3f(glGetUniformLocation(program, "color"), color.r, color.g, color.b);
    glUseProgram(InvalidId);
}
