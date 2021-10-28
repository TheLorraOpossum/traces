#ifndef TRACES_RENDER_H
#define TRACES_RENDER_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t ScenarioHandle;
#ifndef SCENARIO_HANDLE_INVALID
#define SCENARIO_HANDLE_INVALID 0
#endif

struct TracesScenarioOptions
{
    int width;
    int height;

    size_t maxTraces;
    size_t initialTraceCount;

    float splitProbability;
    size_t stepPeriodMs;

    float traceBlurStandardDeviation;

    float colorR;
    float colorG;
    float colorB;
};

ScenarioHandle newScenario(struct TracesScenarioOptions c_options);
void           releaseScenario(ScenarioHandle handle);
void           stepScenario(ScenarioHandle handle);
void           drawScenario(ScenarioHandle handle);

#ifdef __cplusplus
}
#endif


#endif // TRACES_RENDER_H
