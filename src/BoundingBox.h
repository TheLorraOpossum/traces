#pragma once

#include <glm/glm.hpp>
#include <algorithm>

struct BoundingBox
{
    BoundingBox(glm::vec2 const& topLeft_, glm::vec2 const& bottomRight_)
        : topLeft{topLeft_}
        , bottomRight{bottomRight_}
    {}

    static BoundingBox make(glm::vec2 const& center, glm::vec2 const& size)
    {
        glm::vec2 topRight = 0.5f * size;
        glm::vec2 topLeft = glm::vec2(-topRight.x, topRight.y);
        glm::vec2 bottomRight = glm::vec2(topRight.x, -topRight.y);
        return BoundingBox{topLeft, bottomRight}.offsetTo(center);
    }

    glm::vec2 topLeft;
    glm::vec2 bottomRight;

    glm::vec2 size() const {
        return glm::vec2{
          bottomRight.x - topLeft.x,
          topLeft.y - bottomRight.y  
        };
    }

    BoundingBox intersect(BoundingBox const& bb)
    {
        float left = std::max(topLeft.x, bb.topLeft.x);
        float right = std::min(bottomRight.x, bb.bottomRight.x);
        float top = std::min(topLeft.y, bb.topLeft.y);
        float bottom = std::max(bottomRight.y, bb.bottomRight.y);

        return BoundingBox{glm::vec2{left, top},
                           glm::vec2{right, bottom}};
    }

    BoundingBox offsetTo(glm::vec2 const& v)
    {
        return BoundingBox{
            topLeft + v,
            bottomRight + v
        };
    }

    BoundingBox scale(float factor)
    {
        glm::vec2 v = 0.5f * (topLeft + bottomRight);
        BoundingBox onOrigin = this->offsetTo(-v);
        BoundingBox scaled{factor * onOrigin.topLeft, factor * onOrigin.bottomRight};
        return scaled.offsetTo(v);
    }
};