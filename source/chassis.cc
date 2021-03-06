#include <stdexcept>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "chassis.hh"

bool Chassis::built = false;

Chassis::Chassis(unsigned width, unsigned height, ColorRGB clear_color)
    : WIDTH{width}, HEIGHT{height}, clear_color{clear_color} {
    if (built)
        throw std::runtime_error("Chassis already built");
    
    if (!glfwInit())
        throw std::runtime_error("Failed to start GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    glDebugMessageCallback(dbgCallback, nullptr);

    reset_clear_color();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    built = true;
}

GLFWwindow* Chassis::get_window() const {
    return window;
}

void Chassis::reset_clear_color() {
    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
}

Chassis::~Chassis() {
    glfwDestroyWindow(window);
    glfwTerminate();
    
    built = false;
}

void Chassis::dbgCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    if (type == GL_DEBUG_TYPE_ERROR) {
        std::cerr << "[ OpenGL ] " << message << std::endl;
    }
}
