#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

struct Trace
{
    Trace(glm::vec2 const& initialPosition, std::shared_ptr<const GLuint> pProgram, std::shared_ptr<const GLuint> pBuffer);

    void step(std::chrono::milliseconds const& ms);

    void render();

    std::pair<std::shared_ptr<Trace>, std::shared_ptr<Trace>> split() const;

    bool isDead() const;

    glm::vec2 position_;
    glm::vec3 color_;
    glm::vec2 prevPosition_;
    std::shared_ptr<GLuint const> pProgram_;
    std::shared_ptr<GLuint const> pBuffer_;
    std::chrono::steady_clock::time_point creationTime_;
    std::chrono::steady_clock::time_point deathTime_;
    std::size_t id_;

    static glm::vec2 const kMaxStepBoxSizePerMs;
    static std::size_t nextId_;

    friend std::ostream& operator<<(std::ostream& str, Trace const& t);
};

std::ostream& operator<<(std::ostream& str, Trace const& t);