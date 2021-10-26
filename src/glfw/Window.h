#pragma once

#include <chrono>
#include <optional>

struct GLFWwindow;

namespace glfw {
struct Window
{
    Window(int width, int height, std::string const& title, bool borderless);

    Window(int width, int height);
    Window(int width, int height, std::string const& title);

    Window(int width, int height, bool borderless);

    virtual ~Window();

    void select();

    void show();

    bool shouldClose();

    void pollEvents();

    void present();

    void setFullscreen(bool fullscreen);

    operator bool();

    GLFWwindow* pWindow_{};
    std::optional<std::chrono::steady_clock::time_point> lastSwapTime_;
    std::chrono::milliseconds swapPeriodMs_;
    int width_;
    int height_;
};
} // namespace glfw
