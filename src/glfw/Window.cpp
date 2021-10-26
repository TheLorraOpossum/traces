#include "Window.h"
#include "../Utils.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>

using namespace glfw;

Window::Window(int width, int height, std::string const& title, bool borderless)
    : swapPeriodMs_{periodMs()}
    , width_{width}
    , height_{height}
{
    if (borderless)
    {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    //glfwWindowHint(GLFW_SAMPLES, 4);
    pWindow_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    select();
    glewInit();
    glfwSwapInterval(1);
}

Window::Window(int width, int height)
    : Window{width, height, std::string{}, false}
{}

Window::Window(int width, int height, std::string const& title)
    : Window{width, height, title, false}
{}

Window::Window(int width, int height, bool borderless)
    : Window{width, height, "", borderless}
{}

Window::~Window()
{
    if (!pWindow_) return;
    glfwDestroyWindow(pWindow_);
}

void Window::select()
{
    if (!pWindow_) return;
    glfwMakeContextCurrent(pWindow_);
}

void Window::show()
{
    if (!pWindow_) return;
    glfwShowWindow(pWindow_);
}

bool Window::shouldClose()
{
    if (!pWindow_) return true;
    return glfwWindowShouldClose(pWindow_) > 0;
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::present()
{
    if (!pWindow_) return;
//    auto now = std::chrono::steady_clock::now();
//    if (!lastSwapTime_ || (now - lastSwapTime_.value()) >= swapPeriodMs_)
//    {
//        lastSwapTime_ = now;
//        glfwSwapBuffers(pWindow_);
//        return;
//    }
//    std::this_thread::sleep_until(lastSwapTime_.value() + swapPeriodMs_);
    glfwSwapBuffers(pWindow_);
}

void Window::setFullscreen(bool fullscreen)
{
    if (!pWindow_) return;
    glfwSetWindowMonitor(pWindow_,
                         (fullscreen ? glfwGetPrimaryMonitor() : nullptr),
                         0, 0, width_, height_,
                         GLFW_DONT_CARE);
}

Window::operator bool()
{
    return pWindow_ != nullptr;
}
