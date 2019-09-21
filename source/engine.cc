#include <iostream>
#include <stdexcept>
#include <glad/glad.h>
#include "engine.hh"

bool Engine::engine_started = false;

Engine::Engine(Chassis& chassis)
    : chassis{chassis}, mr{"res/shaders/model.vert", "res/shaders/model.frag"}, gui{*this} {
    glfwSetWindowUserPointer(chassis.get_window(), &mr);
    glfwSetKeyCallback(chassis.get_window(), mr.handle_key_input);
    glfwSetCursorPosCallback(chassis.get_window(), mr.handle_mouse_input);
}

void Engine::run_loop() {
    while (!glfwWindowShouldClose(chassis.get_window())) {
        glClear(GL_COLOR_BUFFER_BIT);

        mr.render();
        gui.render();

        glfwSwapBuffers(chassis.get_window());
        glfwPollEvents();
    }
}

void Engine::load_model(std::string const& path) {
    mr.load_model(path);
}

Engine::Gui::Gui(Engine& ngn)
    : ngn{ngn} { }

void Engine::Gui::render() {
    render_model_info();
}

void Engine::Gui::render_model_info() {
    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Models");

    {
        ImGui::InputText("Model Name", model_name_buf, 32);
        if (ImGui::Button("Load Model")) {
            try {
                ngn.load_model(std::string("res/models/") + model_name_buf);
            } catch (std::runtime_error& e) {
                load_info = e.what();
            }
        }
        if (!load_info.empty()) {
            ImGui::Text(load_info.data());
        }
    }
    
    ImGui::End();

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
