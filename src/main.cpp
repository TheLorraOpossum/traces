#include "BulkRenderer.h"
#include "DoubleFramebuffer.h"
#include "Error.h"
#include "Scenario.h"
#include "Trace.h"
#include "TraceFactory.h"
#include "Utils.h"
#include "glfw/Lifecycle.h"
#include "glfw/Window.h"


#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    glfw::Lifecycle lc;
    Error err;
    glm::ivec2 videoMode;
    std::tie(videoMode, err) = getMonitorCurrentVideoMode();
    if (err != nil)
    {
        std::cout << "could not get current monitor video mode:" << err.value();
        return -2;
    }

    glfw::Window w{videoMode.x, videoMode.y, true};
    w.setFullscreen(true);
    float widthOverHeight = static_cast<float>(videoMode.x) / videoMode.y;
    float heightOverWidth = 1.0f/widthOverHeight;

    constexpr std::size_t kMaxTraces{200};

    w.select();

    std::shared_ptr<Scenario> pScenario;
    Scenario::Options options;
    options.color = glm::vec3{0.0, 1.0, 1.0};
    std::tie(pScenario, err) = Scenario::make(20, videoMode, options);
    if (err != nil)
    {
        std::cout << "could not make Scenario: " << err.value();
        return -2;
    }

    w.show();


    std::optional<std::chrono::steady_clock::time_point> then;
    std::size_t missedDeadlines = 0;
    std::size_t totalIterations = 0;

    bool missedDeadlinesPrinted{false};
    int partialIterations{0};
    std::optional<std::chrono::steady_clock::time_point> lastFpsPrint;

    while (w && !w.shouldClose())
    {
        ++totalIterations;
        ++partialIterations;
        auto now = std::chrono::steady_clock::now();
        if (then)
        {
            auto overTime = (now - then.value()).count() / 1'000'000 - periodMs().count();
            if (overTime > 0)
            {
                missedDeadlinesPrinted = false;
                missedDeadlines++;
            }
            if (missedDeadlines % 50 == 0)
            {
                missedDeadlinesPrinted = true;
                std::cout << now.time_since_epoch().count() / 1'000'000 << " MISSED DEADLINES " //<< std::setw(5) << std::setprecision(5)
                          << 100.0 * static_cast<double>(missedDeadlines) / totalIterations << "%" << std::endl;
            }
            auto fpsInterval = now - lastFpsPrint.value_or(now);
            if (!lastFpsPrint || fpsInterval >= 1s)
            {
                std::cout << now.time_since_epoch().count() / 1e9 << " " << partialIterations / (fpsInterval.count()/1e9) << "fps" << std::endl;
                partialIterations = 0;
                lastFpsPrint = now;
            }
        }
        then = now;
        w.pollEvents();

        pScenario->step();
        pScenario->draw();

        w.present();
    }

    return 0;
}
