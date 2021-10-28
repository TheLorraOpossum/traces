#pragma once

#include "BoundingBox.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

struct Trace
{
    Trace(glm::vec2 const& initialPosition, float initialDirection_, const glm::vec3 &color, std::chrono::steady_clock::time_point const& creationTime, std::shared_ptr<const GLuint> pProgram, std::shared_ptr<const GLuint> pBuffer);

    void step(std::chrono::milliseconds const& ms);

    void render();

    std::pair<std::shared_ptr<Trace>, std::shared_ptr<Trace>> split() const;

    bool isDead() const;
    void kill();

    float prevTheta() const;
    float uniformAround(float thetaCenter, float thetaWidth) const;

    glm::vec2 position_;
    float direction_;
    float speed_;
    glm::vec3 color_;
    glm::vec2 prevPosition_;
    std::shared_ptr<GLuint const> pProgram_;
    std::shared_ptr<GLuint const> pBuffer_;
    std::chrono::steady_clock::time_point creationTime_;
    std::chrono::steady_clock::time_point deathTime_;
    bool killed_;
    std::size_t id_;

    static float const kMaxStepMagnitude;
    static std::size_t nextId_;

    friend std::ostream& operator<<(std::ostream& str, Trace const& t);
};

std::ostream& operator<<(std::ostream& str, Trace const& t);
