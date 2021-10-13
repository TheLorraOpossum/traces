#pragma once

#include <optional>
#include <sstream>
#include <string>

using Error = std::optional<std::string>;
inline const Error nil = std::nullopt;

template<typename ...Args>
Error makeError(Args ... args)
{
    std::ostringstream stream;
    (stream << " " << ... << args);
    return stream.str();
}