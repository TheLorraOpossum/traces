#pragma once

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

struct Trace;

struct BulkRenderer
{
    BulkRenderer(GLuint const& traceProgram, GLuint const& vertexBuffer);
    ~BulkRenderer() = default;

    void add(std::shared_ptr<Trace> pTrace);
    void bufferData();
    void render();

    void setColor(glm::vec3 const& color);

private:
    GLuint const& program;
    GLuint const& buffer;
    std::vector<std::shared_ptr<Trace>> vpTraces;
};
