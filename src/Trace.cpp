#include "Trace.h"
#include "TraceFactory.h"
#include "Utils.h"
#include <random>

Trace::Trace(glm::vec2 const& initialPosition, std::shared_ptr<const GLuint> pProgram, std::shared_ptr<const GLuint> pBuffer, BoundingBox const& allowedBox)
    : position_{initialPosition}
    , color_{1.0, 0.0, 1.0}
    , pProgram_{pProgram}
    , pBuffer_{pBuffer}
    , allowedBox_{allowedBox}
    , creationTime_{std::chrono::steady_clock::now()}
    , deathTime_{creationTime_}
    , id_{++nextId_}
{
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dis{1000, 5000};
    deathTime_ += std::chrono::milliseconds{dis(gen)};
    step(std::chrono::milliseconds{1});
}

void Trace::step(std::chrono::milliseconds const &ms)
{
    prevPosition_ = position_;

    glm::vec2 const maxStepBoxSizeHalf = kMaxStepBoxSizePerMs * static_cast<float>(ms.count());
    glm::vec2 const topLeft = glm::vec2{-maxStepBoxSizeHalf.x, maxStepBoxSizeHalf.y};
    glm::vec2 const bottomRight = glm::vec2{maxStepBoxSizeHalf.x, -maxStepBoxSizeHalf.y};

    BoundingBox stepBox{BoundingBox{topLeft, bottomRight}.offsetTo(position_).intersect(allowedBox_)};

    position_ = uniformInBox(stepBox); // TODO: change to +=

    //std::cout << "Trace::step() " << *this << std::endl;
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
        std::make_shared<Trace>(position_, pProgram_, pBuffer_, allowedBox_),
        std::make_shared<Trace>(position_, pProgram_, pBuffer_, allowedBox_)
    );
}


bool Trace::isDead() const
{
    return std::chrono::steady_clock::now() >= deathTime_;
}

std::ostream &operator<<(std::ostream &str, Trace const &t)
{
    str << "Trace{id: " << t.id_ << " position: (" << t.position_.x << "," << t.position_.y << ")"
        << " prevPosition: (" << t.prevPosition_.x << "," << t.prevPosition_.y << ")"
        << " color: (" << t.color_.r << "," << t.color_.g << "," << t.color_.b << ")";

    return str;
}

glm::vec2 const Trace::kMaxStepBoxSizePerMs{0.001, 0.001};
std::size_t Trace::nextId_{0};