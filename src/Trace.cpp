#include "Trace.h"
#include "TraceFactory.h"
#include "Utils.h"
#include <glm/gtx/polar_coordinates.hpp>
#include <random>

Trace::Trace(glm::vec2 const& initialPosition, float initialDirection_, std::shared_ptr<const GLuint> pProgram, std::shared_ptr<const GLuint> pBuffer)
    : position_{initialPosition}
    , direction_{initialDirection_}
    , color_{1.0, 0.0, 1.0}
    , pProgram_{pProgram}
    , pBuffer_{pBuffer}
    , creationTime_{std::chrono::steady_clock::now()}
    , deathTime_{creationTime_}
    , killed_{false}
    , id_{++nextId_}
{
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::geometric_distribution<> dis;
    std::normal_distribution<> nd{1.0, 0.25};
    //deathTime_ += std::chrono::milliseconds{dis(gen)*2000};
    deathTime_ += std::chrono::milliseconds{static_cast<int>(nd(gen)*800)};
    speed_ = nd(gen);
    step(periodMs());
}

void Trace::step(std::chrono::milliseconds const &ms)
{
    float newDirection = uniformAround(direction_, 3 * M_PI / 36.0);

    glm::vec2 deltaPositionPolar = glm::vec2(
        speed_ * kMaxStepMagnitude * ms.count(),
        newDirection);

    glm::vec2 deltaPosition{
        deltaPositionPolar.x * cos(deltaPositionPolar.y),
        deltaPositionPolar.x * sin(deltaPositionPolar.y)};

    prevPosition_ = position_;
    direction_ = newDirection;
    position_ += deltaPosition;
}

void Trace::render()
{
    if (!pProgram_)
        return;
    glUseProgram(*pProgram_);

    glBindBuffer(GL_ARRAY_BUFFER, *pBuffer_);
    std::vector<float> vs{prevPosition_.x, prevPosition_.y, position_.x, position_.y};
    glBufferData(GL_ARRAY_BUFFER, static_cast<int>(vs.size()) * sizeof(vs[0]), vs.data(), GL_STREAM_DRAW);

    GLint positionLocation = glGetAttribLocation(*pProgram_, "position");
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(positionLocation);

    glUniform3f(glGetUniformLocation(*pProgram_, "color"), color_.r, color_.g, color_.b);

    glDrawArrays(GL_LINES, 0, 4);

    glDisableVertexAttribArray(positionLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

std::pair<std::shared_ptr<Trace>, std::shared_ptr<Trace>> Trace::split() const
{
    return std::make_pair(
        std::make_shared<Trace>(position_, uniformAround(direction_, M_PI / 4), pProgram_, pBuffer_),
        std::make_shared<Trace>(position_, uniformAround(direction_, M_PI / 4), pProgram_, pBuffer_)
    );
}


bool Trace::isDead() const
{
    return killed_ || std::chrono::steady_clock::now() >= deathTime_;
}

void Trace::kill()
{
    killed_ = true;
}

float Trace::prevTheta() const
{
    glm::vec2 prevDeltaPosition = position_ - prevPosition_;
    return atan2(prevDeltaPosition.y, prevDeltaPosition.x);
}

float Trace::uniformAround(float thetaCenter, float thetaWidth) const
{
    return uniformInInterval(
        thetaCenter - thetaWidth / 2.0,
        thetaCenter + thetaWidth / 2.0);
}

std::ostream &operator<<(std::ostream &str, Trace const &t)
{
    str << "Trace{id: " << t.id_ << " position: (" << t.position_.x << "," << t.position_.y << ")"
        << " prevPosition: (" << t.prevPosition_.x << "," << t.prevPosition_.y << ")"
        << " color: (" << t.color_.r << "," << t.color_.g << "," << t.color_.b << ")";

    return str;
}

float const Trace::kMaxStepMagnitude{0.1f/(periodMs().count() * 60)};
std::size_t Trace::nextId_{0};
