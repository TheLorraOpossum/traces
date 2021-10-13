#include "Shader.h"
#include "Utils.h"

#include <tuple>
#include <vector>

Error compileShader(GLuint id, std::string const &source);
std::pair<std::string, Error> getShaderInfoLog(GLuint id);
std::shared_ptr<const GLuint> makeSharedPtr(GLuint shaderId);

GLenum guessShaderType(std::string const &path)
{
    std::size_t lastDot = path.rfind('.');
    if (lastDot == std::string::npos)
        return GL_INVALID_ENUM;
    std::string ext = path.substr(lastDot);
    if (ext == ".vert")
        return GL_VERTEX_SHADER;
    if (ext == ".frag")
        return GL_FRAGMENT_SHADER;
    return GL_INVALID_ENUM;
}

std::pair<std::shared_ptr<const GLuint>, Error> loadShader(std::string const &path)
{
    GLenum shaderType = guessShaderType(path);
    if (shaderType == GL_INVALID_ENUM)
    {
        return std::make_pair(nullptr, makeError("could not guess shader type for", path));
    }
    auto [contents, err] = readFile(path);
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not read", path, ":", err.value()));
    }
    std::shared_ptr<const GLuint> pId;
    std::tie(pId, err) = makeShader(contents, shaderType);
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not make shader from", path, ":", err.value()));
    }
    return std::make_pair(pId, nil);
}

std::tuple<std::shared_ptr<const GLuint>, std::shared_ptr<const GLuint>, Error> loadShaderPair(std::string const &directory, std::string const &name)
{
    std::vector<std::shared_ptr<const GLuint>> ids;
    for (std::string ext : {".vert", ".frag"})
    {
        std::string path = directory + "/" + name + ext;
        auto const [pId, err] = loadShader(path);
        if (err != nil)
        {
            return std::make_tuple(nullptr, nullptr,
                                   makeError("could not load shader pair, failed loading:", err.value()));
        }
        ids.push_back(pId);
    }
    return std::make_tuple(ids[0], ids[1], nil);
}

std::pair<std::shared_ptr<const GLuint>, Error> makeShader(std::string const &source, GLenum type)
{
    GLuint id = glCreateShader(type);
    if (id == InvalidId)
    {
        return std::make_pair(nullptr, makeError("could not glCreateShader() for shader type", type));
    }
    Error err = compileShader(id, source);
    if (err == nil)
    {
        return std::make_pair(makeSharedPtr(id), nil);
    }
    glDeleteShader(id);
    return std::make_pair(nullptr, makeError("could not make shader:", err.value()));
}

Error compileShader(GLuint id, std::string const &source)
{
    int len = static_cast<int>(source.size());
    char const *pSource = source.data();
    glShaderSource(id, 1, &pSource, &len);
    glCompileShader(id);
    GLint ok{-1};
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (ok == GL_TRUE)
    {
        return nil;
    }
    auto [infoLog, err] = getShaderInfoLog(id);
    if (err == nil)
    {
        return makeError("could not compile shader:", infoLog);
    }
    return makeError("could not compile shader, nor could the info log be retrieved:", err.value());
}

std::pair<std::string, Error> getShaderInfoLog(GLuint id)
{
    GLint infoLogLength{-1};
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength == -1)
    {
        return std::make_pair("", makeError("could not retrieve shader info log length"));
    }
    if (infoLogLength == 0)
    {
        return std::make_pair("", makeError("shader info log length was 0"));
    }
    char *pInfoLog = new (std::nothrow) char[infoLogLength];
    if (!pInfoLog)
    {
        return std::make_pair("", makeError("no memory for shader info log"));
    }
    glGetShaderInfoLog(id, infoLogLength, nullptr, pInfoLog);
    std::string infoLog = pInfoLog;
    delete[] pInfoLog;

    return std::make_pair(infoLog, nil);
}

struct GLuintDeleter : public std::default_delete<GLuint>
{
    void operator()(GLuint *pId) const
    {
        if (!pId)
            return;
        glDeleteShader(*pId);
    }
};

std::shared_ptr<const GLuint> makeSharedPtr(GLuint shaderId)
{
    return std::make_shared<const GLuint>(shaderId);
}