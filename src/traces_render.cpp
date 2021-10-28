#include "traces_render.h"

#include "Error.h"
#include "Scenario.h"

#include <glm/glm.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <utility>
#include <unordered_map>

static std::unordered_map<ScenarioHandle, std::shared_ptr<Scenario>> g_mapScenarios;
static std::size_t nextHandle{0};

Scenario::Options toScenarioOptions(TracesScenarioOptions const& c_options)
{
    Scenario::Options options;
    options.color = glm::vec3{c_options.colorR, c_options.colorG, c_options.colorB};
    options.maxTraces = c_options.maxTraces;
    options.splitProbability = c_options.splitProbability;

    options.stepPeriod = std::chrono::milliseconds{c_options.stepPeriodMs};

    return options;
}


ScenarioHandle newScenario(TracesScenarioOptions c_options)
{
    std::shared_ptr<Scenario> pScenario;
    Error err;
    Scenario::Options options{toScenarioOptions(c_options)};
    std::tie(pScenario, err) = Scenario::make(c_options.initialTraceCount,
                                              glm::ivec2{c_options.width, c_options.height},
                                              options);
    if (err != nil)
    {
        std::cerr << "could not create Scenario: " << err.value() << std::endl;
        return SCENARIO_HANDLE_INVALID;
    }
    auto usedHandle = ++nextHandle;
    g_mapScenarios[usedHandle] = pScenario;
    return usedHandle;
}

void releaseScenario(ScenarioHandle handle)
{
    auto itScenario = g_mapScenarios.find(handle);
    if (itScenario != g_mapScenarios.end())
    {
        g_mapScenarios.erase(itScenario);
    }
}

void stepScenario(ScenarioHandle handle)
{
    auto itScenario = g_mapScenarios.find(handle);
    if (itScenario != g_mapScenarios.end())
    {
        itScenario->second->step();
    }
}

void drawScenario(ScenarioHandle handle)
{
    auto itScenario = g_mapScenarios.find(handle);
    if (itScenario != g_mapScenarios.end())
    {
        itScenario->second->draw();
    }
}
