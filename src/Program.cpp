#include "Program.h"
#include "Utils.h"

Error linkProgram(GLuint id);
std::pair<std::string, Error> getProgramLinkLog(GLuint id);

std::pair<std::shared_ptr<const GLuint>, Error> makeProgram(std::shared_ptr<const GLuint> pVert, std::shared_ptr<const GLuint> pFrag)
{
    GLuint id = glCreateProgram();
    if (id == InvalidId)
    {
        return std::make_pair(nullptr, makeError("could not glCreateProgram()"));
    }
    for (auto const pShaderId : {pVert, pFrag}) glAttachShader(id, *pShaderId);
    auto err = linkProgram(id);
    if (err == nil)
    {
        std::shared_ptr<const GLuint> pProgram;
        auto lDeleter = [](GLuint* pId)
        {
            if (!pId) return;
            glDeleteProgram(*pId);
        };
        pProgram.reset(new (std::nothrow)GLuint(id), lDeleter);
        if (!pProgram)
        {
            return std::make_pair(nullptr, makeError("could not instantiate program (new gave nullptr)"));
        }
        return std::make_pair(pProgram, nil);
    }
    return std::make_pair(nullptr, err);
}

Error linkProgram(GLuint id)
{
    if (id == InvalidId) return makeError("linkProgram() invalid ID passed in");
    glLinkProgram(id);
    GLint ok{-1};
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (ok == -1) return makeError("could not retrieve program link status");
    if (ok == GL_TRUE) return nil;
    auto const [linkLog, err] = getProgramLinkLog(id);
    if (err == nil) return makeError("could not link program with ID", id, ":", linkLog);
    return makeError("could not link program with ID", id, " and could not retrieve link log:", err.value());
}

std::pair<std::string, Error> getProgramLinkLog(GLuint id)
{
    if (id == InvalidId) return std::make_pair("", makeError("getProgramLinkLog(): invalid ID passed in"));

    GLint linkLogLength{-1};
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &linkLogLength);
    if (linkLogLength == -1) return std::make_pair("", makeError("could not retrieve linkLogLength for program with ID", id));
    if (linkLogLength == 0) return std::make_pair("", makeError("retrieved linkLogLength for program with ID", id, "was 0"));

    char *pLinkLog = new (std::nothrow)char[linkLogLength];
    if (!pLinkLog) return std::make_pair("", makeError("could not instantiate buffer for linkLog for program with ID", id));

    glGetProgramInfoLog(id, linkLogLength, nullptr, pLinkLog);
    std::string linkLog = pLinkLog;
    delete[] pLinkLog;

    return std::make_pair(linkLog, nil);
}