#include "Scenario.h"
#include "BoundingBox.h"
#include "BulkRenderer.h"
#include "DoubleFramebuffer.h"
#include "Trace.h"
#include "TraceFactory.h"
#include "Utils.h"


std::pair<std::shared_ptr<Scenario>, Error> Scenario::make(std::size_t initialTraceCount, const glm::ivec2 &windowSize)
{
    return make(initialTraceCount, windowSize, nullptr);
}

std::pair<std::shared_ptr<Scenario>, Error> Scenario::make(std::size_t initialTraceCount, const glm::ivec2 &windowSize, std::shared_ptr<Options> pOptions)
{
    auto pScenario = std::make_shared<Scenario>();
    if (!pScenario) return std::make_pair(nullptr, makeError("could not instantiate Scenario"));
    if (pOptions != nullptr)
    {
        pScenario->m_options = *pOptions;
    }
    float windowHeightOverWidth = static_cast<float>(windowSize.y) / windowSize.x;
    Error err;
    std::tie(pScenario->m_pTraceFactory, err) = TraceFactory::getInstance(windowHeightOverWidth);
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not build Scenario:", err.value()));
    }
    pScenario->m_pBulkRenderer = pScenario->m_pTraceFactory->getBulkRenderer();
    pScenario->m_windowHeightOverWidth = windowHeightOverWidth;
    pScenario->m_windowBoundariesMonometric = WindowBoundaries{windowHeightOverWidth};


    std::tie(pScenario->m_pDoubleFramebuffer, err) = DoubleFramebuffer::get(windowSize.x, windowSize.y);
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not make scenario:", err.value()));
    }

    pScenario->genTraces(initialTraceCount);

    return std::make_pair(pScenario, nil);
}

std::pair<std::shared_ptr<Scenario>, Error> Scenario::make(std::size_t initialTraceCount, const glm::ivec2 &windowSize, Options options)
{
    auto pScenario = std::make_shared<Scenario>();
    if (!pScenario) return std::make_pair(nullptr, makeError("could not instantiate Scenario"));

    pScenario->m_options = options;
    float windowHeightOverWidth = static_cast<float>(windowSize.y) / windowSize.x;
    Error err;
    std::tie(pScenario->m_pTraceFactory, err) = TraceFactory::getInstance(windowHeightOverWidth);
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not build Scenario:", err.value()));
    }
    pScenario->m_pBulkRenderer = pScenario->m_pTraceFactory->getBulkRenderer();
    pScenario->m_windowHeightOverWidth = windowHeightOverWidth;
    pScenario->m_windowBoundariesMonometric = WindowBoundaries{windowHeightOverWidth};


    std::tie(pScenario->m_pDoubleFramebuffer, err) = DoubleFramebuffer::get(windowSize.x, windowSize.y);
    if (err != nil)
    {
        return std::make_pair(nullptr, makeError("could not make scenario:", err.value()));
    }

    pScenario->genTraces(initialTraceCount);

    return std::make_pair(pScenario, nil);
}

void Scenario::genTraces(int count)
{
    auto now = std::chrono::steady_clock::now();
    for (int i = 0; i < count; i++)
    {
        auto [pTrace, err] = m_pTraceFactory->make(BoundingBox{glm::vec2{-1.0, 1.0}, glm::vec2{1.0, -1.0}}, m_options.color, now);
        if (err != nil)
        {
            std::cout << "could not make trace: " << err.value() << std::endl;
            continue;
        }
        m_vpTraces.push_back(pTrace);
    }
}

Scenario::WindowBoundaries::WindowBoundaries()
    : BoundingBox{glm::vec2{}, glm::vec2{}}
{}

Scenario::WindowBoundaries::WindowBoundaries(float windowHeightOverWidth)

    : BoundingBox{glm::vec2{-1.0/windowHeightOverWidth, 1.0},glm::vec2{1.0/windowHeightOverWidth, -1.0}}
{}

Scenario::WindowBoundaries &Scenario::WindowBoundaries::operator=(const WindowBoundaries &rhs)
{
    topLeft = rhs.topLeft;
    bottomRight = rhs.bottomRight;
    return *this;
}

void Scenario::step()
{
    std::vector<std::shared_ptr<Trace>> newTraces;

    auto itTrace = m_vpTraces.begin();
    while (itTrace != m_vpTraces.end())
    {
        auto pTrace = *itTrace;
        bool traceOutOfBoundary = !BoundingBox{m_windowBoundariesMonometric.topLeft, m_windowBoundariesMonometric.bottomRight}.contains(pTrace->position_);
        if (traceOutOfBoundary ||
                (m_vpTraces.size() > (m_options.maxTraces - (m_options.maxTraces / 5))))
        {
            //std::cout << "KILL (" << (traceOutOfBoundary ? "B": "L") << ") " << *pTrace << std::endl;
            pTrace->kill();
            itTrace = m_vpTraces.erase(itTrace);
            continue;
        }
        if (pTrace->isDead())
        {
            //std::cout << "DEAD " << *pTrace << std::endl;
            if (uniformInInterval(0.0, 1.0) > 0.4)
            {
                auto [pTrace1, pTrace2] = pTrace->split();
                newTraces.push_back(pTrace1);
                newTraces.push_back(pTrace2);
            }
            itTrace = m_vpTraces.erase(itTrace);
            continue;
        }
        pTrace->step(m_options.stepPeriod);
        itTrace++;
    }
    for (auto pTrace : newTraces)
    {
        //std::cout << "NEW " << *pTrace << std::endl;
        m_vpTraces.push_back(pTrace);
    }

    if (m_vpTraces.size() == 0)
    {
        genTraces(20);
    }


    if (m_pBulkRenderer)
    {
        for (auto pTrace : m_vpTraces)
        {
            m_pBulkRenderer->add(pTrace);
        }
        m_pBulkRenderer->bufferData();
    }
}

void Scenario::draw()
{
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    m_pDoubleFramebuffer->renderPreviousFrame();
    if (m_pBulkRenderer)
    {
        m_pBulkRenderer->setColor(m_options.color);
        m_pBulkRenderer->render();
    }

    m_pDoubleFramebuffer->blitAndSwap();
}
