#include <stdexcept>
#include <iostream>
#include <glad/glad.h>
#include "chassis.hh"

bool Chassis::built = false;

Chassis::Chassis(int width, int height, ColorRGB color)
    : WIDTH{width}, HEIGHT{height} {
    if (built)
        throw std::runtime_error("Chassis already built");
    
    if (!glfwInit())
        throw std::runtime_error("Failed to start GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "misery engine", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create a window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL functions using glad");
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glClearColor(color.r, color.g, color.b, 1.0f);

    built = true;
    std::cout << "Ye boiiii" << std::endl;
}

GLFWwindow* Chassis::get_window() const {
    return window;
}

Chassis::~Chassis() {
    if (!built)
        throw std::runtime_error("Chassis doesn't exist");
    std::cout << "BRUH?!" << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    built = false;
}
