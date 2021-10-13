#include "DoubleFramebuffer.h"
#include "Program.h"
#include "Shader.h"
#include "Utils.h"
#include <GL/glew.h>

std::pair<std::shared_ptr<DoubleFramebuffer>, Error> DoubleFramebuffer::get(int width, int height)
{
    if (pInstance)
        return std::make_pair(pInstance, nil);
    if (!pInstance && creationError)
        return std::make_pair(nullptr, creationError);

    auto [pProgram, err] = makeQuadRenderProgram();
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not create DoubleFramebuffer instance:", err.value()));
    }

    pQuadRenderProgram = pProgram;
    pQuadBuffer = genBuffer();
    fillBuffer();

    screenSize.x = width;
    screenSize.y = height;

    glGenFramebuffers(2, framebuffers.data());
    glGenTextures(2, textures.data());
    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[i]);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[i], 0);
        if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            glDeleteFramebuffers(2, framebuffers.data());
            glDeleteTextures(2, textures.data());
            auto eError = glGetError();
            auto err = makeError("could not setup framebuffer", i, ":", glewGetErrorString(eError));
            creationError = err;
            return std::make_pair(nullptr, err);
        }
    }
    auto pDoubleFramebuffer = std::make_shared<DoubleFramebuffer>();
    pDoubleFramebuffer.reset(new (std::nothrow) DoubleFramebuffer(), [](DoubleFramebuffer *pDfb)
                             {
                                 glDeleteFramebuffers(2, framebuffers.data());
                                 glDeleteTextures(2, textures.data());
                             });

    pInstance->bindFramebuffer();

    return std::make_pair(pDoubleFramebuffer, nil);
}

void DoubleFramebuffer::renderPreviousFrame()
{
    if (noPreviousFrame) return;
    glUseProgram(*pQuadRenderProgram);

    glBindBuffer(GL_ARRAY_BUFFER, *pQuadBuffer);
    GLint positionLocation = glGetAttribLocation(*pQuadRenderProgram, "position");
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(positionLocation);

    GLint frameTextureLocation = glGetUniformLocation(*pQuadRenderProgram, "frameTexture");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[previousIndex()]);
    glUniform1i(frameTextureLocation, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(positionLocation);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DoubleFramebuffer::blitAndSwap()
{
    noPreviousFrame = false;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers[currentIndex_]);
    glBlitFramebuffer(0, 0, screenSize.x, screenSize.y,
                      0, 0, screenSize.x, screenSize.y,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    currentIndex_ = (currentIndex_ + 1) % 2;

    bindFramebuffer();
}

void DoubleFramebuffer::bindFramebuffer()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[currentIndex_]);
}

std::pair<std::shared_ptr<const GLuint>, Error> DoubleFramebuffer::makeQuadRenderProgram()
{
    auto [pVert, pFrag, err] = loadShaderPair("../src/shaders", "texturedQuad");
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not build texturedQuad program:", err.value()));
    }
    auto [pProgram, err2] = makeProgram(pVert, pFrag);
    if (err2 != nil)
    {
        return std::make_pair(nullptr, makeError("could not build texturedQuad program:", err2.value()));
    }
    return std::make_pair(pProgram, nil);
}

int DoubleFramebuffer::currentIndex()
{
    return currentIndex_;
}

int DoubleFramebuffer::previousIndex()
{
    return (currentIndex_+1)%2;
}

void DoubleFramebuffer::fillBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, *pQuadBuffer);
    std::array<glm::vec2, 4> vs{
        glm::vec2{-1.0, -1.0},
        glm::vec2{ 1.0, -1.0},
        glm::vec2{-1.0,  1.0},
        glm::vec2{ 1.0,  1.0}
    };
    glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(vs[0]), vs.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::array<GLuint, 2> DoubleFramebuffer::textures;
std::array<GLuint, 2> DoubleFramebuffer::framebuffers;
bool DoubleFramebuffer::noPreviousFrame{true};
std::shared_ptr<const GLuint> DoubleFramebuffer::pQuadRenderProgram;
std::shared_ptr<const GLuint> DoubleFramebuffer::pQuadBuffer;
glm::ivec2 DoubleFramebuffer::screenSize;
int DoubleFramebuffer::currentIndex_{0};
std::shared_ptr<DoubleFramebuffer> DoubleFramebuffer::pInstance;
Error DoubleFramebuffer::creationError;