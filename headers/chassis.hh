#pragma once

#define GLFW_INCLUDE_NONE
#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "color.hh"

class Chassis {
public:
    Chassis(int32_t width, int32_t height, ColorRGB color);
    ~Chassis();

    GLFWwindow* get_window() const;

private:
    const int32_t WIDTH;
    const int32_t HEIGHT;

    GLFWwindow* window;

    static bool built;

    static void dbgCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
        const GLchar *message, const void *userParam);
};
