#pragma once

#include "BoundingBox.h"
#include "Error.h"
#include <glm/glm.hpp>
#include <chrono>
#include <memory>
#include <utility>
#include <vector>

struct BulkRenderer;
struct DoubleFramebuffer;
struct Trace;
struct TraceFactory;

struct Scenario
{
    struct Options
    {
        std::size_t maxTraces{200};
        float splitProbability{.4f};
        std::chrono::milliseconds stepPeriod{16};
        glm::vec3 color{1.0, 0.0, 1.0};
    };

    static std::pair<std::shared_ptr<Scenario>, Error> make(std::size_t initialTraceCount, glm::ivec2 const& windowSize);

    static std::pair<std::shared_ptr<Scenario>, Error> make(std::size_t initialTraceCount, glm::ivec2 const& windowSize, std::shared_ptr<Options> pOptions);
    static std::pair<std::shared_ptr<Scenario>, Error> make(std::size_t initialTraceCount, glm::ivec2 const& windowSize, Options options);


    void genTraces(int count);

    Options m_options;
    std::vector<std::shared_ptr<Trace>> m_vpTraces;
    std::shared_ptr<TraceFactory> m_pTraceFactory;
    std::shared_ptr<BulkRenderer> m_pBulkRenderer;
    std::shared_ptr<DoubleFramebuffer> m_pDoubleFramebuffer;
    float m_windowHeightOverWidth;
    struct WindowBoundaries : public BoundingBox
    {
        WindowBoundaries();
        WindowBoundaries(float windowHeightOverWidth);
        WindowBoundaries& operator=(WindowBoundaries const& rhs);
    };

    void step();

    void draw();
    
    WindowBoundaries m_windowBoundariesMonometric;
};
