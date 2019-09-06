#pragma once

#define GLFW_INCLUDE_NONE
#include <cstdint>
#include <GLFW/glfw3.h>
#include "color.hh"

class Chassis {
public:
    Chassis(int32_t width, int32_t height, ColorRGB color);
    ~Chassis();

    Chassis(Chassis const&) = delete;
    Chassis& operator=(Chassis const&) = delete;
    Chassis(Chassis&&);
    Chassis& operator=(Chassis&&) = delete;

    GLFWwindow* get_window() const;

private:
    const int32_t WIDTH;
    const int32_t HEIGHT;

    GLFWwindow* window;

    static bool built, moving;
};
