#pragma once

#include "Error.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <array>
#include <memory>
#include <utility>


struct DoubleFramebuffer
{
    static std::pair<std::shared_ptr<DoubleFramebuffer>, Error> get(int width, int height);

    void renderPreviousFrame();

    void blitAndSwap();

    void bindFramebuffer();

    static std::pair<std::shared_ptr<const GLuint>, Error> makeQuadRenderProgram();
    int currentIndex();
    int previousIndex();
    static void fillBuffer();

    static std::array<GLuint, 2> textures; 
    static std::array<GLuint, 2> framebuffers;
    static bool noPreviousFrame;
    static std::shared_ptr<const GLuint> pQuadRenderProgram;
    static std::shared_ptr<const GLuint> pQuadBuffer;
    static glm::ivec2 screenSize;
    static int currentIndex_;
    static std::shared_ptr<DoubleFramebuffer> pInstance;
    static Error creationError;
};
