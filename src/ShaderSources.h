#pragma once

#include <string>

inline std::string const texturedQuad_frag{
    #include "shaders/texturedQuad.frag"
};

inline std::string const textureQuad_vert{
    #include "shaders/texturedQuad.vert"
};

inline std::string const trace_frag{
    #include "shaders/trace.frag"
};

inline std::string const trace_vert{
    #include "shaders/trace.vert"
};