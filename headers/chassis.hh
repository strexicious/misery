#pragma once

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "color.hh"

class Chassis {
public:

    Chassis(unsigned width, unsigned height, ColorRGB color);
    ~Chassis();

    GLFWwindow* get_window() const;

    void reset_clear_color();

    unsigned get_width() const { return WIDTH; }
    unsigned get_height() const { return HEIGHT; }

private:

    const unsigned WIDTH;
    const unsigned HEIGHT;

    GLFWwindow* window;
    ColorRGB clear_color;

    static bool built;

    static void dbgCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
        const GLchar *message, const void *userParam);

};
