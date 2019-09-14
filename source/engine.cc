#include <stdexcept>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "engine.hh"

bool Engine::engine_started = false;

Engine::Engine(Chassis& chassis)
    : chassis{chassis},
      shader{read_shader("res/shaders/shader1.vert"), read_shader("res/shaders/shader1.frag")} { }

void Engine::run_loop() const {
    glUseProgram(shader.id());

    while (!glfwWindowShouldClose(chassis.get_window())) {
        glClear(GL_COLOR_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Demo window");
        ImGui::Button("Hello!");
        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(chassis.get_window());
        glfwPollEvents();
    }
}
