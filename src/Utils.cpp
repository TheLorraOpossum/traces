#include "Utils.h"
#include <fstream>
#include <random>

namespace {
std::random_device rd;
std::mt19937 gen{rd()};
}

std::pair<std::string, Error> readFile(std::string const& path)
{
    std::ifstream f;
    f.open(path, std::ios::in);
    if (!f.is_open())
    {
        return std::make_pair(std::string{""}, makeError("could not open file:", path));
    }
    std::ostringstream stream;
    stream << f.rdbuf();

    return std::make_pair(stream.str(), nil);
}

glm::vec2 uniformInBox(glm::vec2 const& boundTopLeft, glm::vec2 const& boundBottomRight)
{
    std::uniform_real_distribution<float> unifX{boundTopLeft.x, boundBottomRight.x};
    std::uniform_real_distribution<float> unifY{boundBottomRight.y, boundTopLeft.y};
    return glm::vec2{unifX(gen), unifY(gen)};
}

glm::vec2 uniformInBox(BoundingBox const& bb)
{
    return uniformInBox(bb.topLeft, bb.bottomRight);
}

float uniformInInterval(float left, float right)
{
    std::uniform_real_distribution<float> dis{left, right};
    return dis(gen);
}

std::shared_ptr<GLuint const> genBuffer()
{
    GLuint buffer{InvalidId};
    glGenBuffers(1, &buffer);

    auto lDeleter = [](GLuint *pId)
    {
        if (!pId)
            return;
        glDeleteBuffers(1, pId);
    };

    std::shared_ptr<GLuint> pBufferLocal;

    pBufferLocal.reset(new (std::nothrow) GLuint(buffer), lDeleter);
    return pBufferLocal;
}

std::chrono::milliseconds periodMs()
{
    return std::chrono::milliseconds{16};
}