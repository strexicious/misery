#include <iostream>
#include <stdexcept>
#include <functional>
#include <glad/glad.h>
#include "engine.hh"

bool Engine::engine_started = false;

Engine::Engine(Chassis& chassis)
    : chassis{chassis}, mr{"res/shaders/model.vert", "res/shaders/model.frag"}, gui{*this} {
    glfwSetWindowUserPointer(chassis.get_window(), &ih);
    glfwSetKeyCallback(chassis.get_window(), &InputHandler::glfw_keyboard_handler);
    glfwSetCursorPosCallback(chassis.get_window(), &InputHandler::glfw_mouse_handler);

    ih.set_press_handler("engine_exploration", std::function<void(int)>(
        [this](int key) {
            if (key == GLFW_KEY_ESCAPE) {
                set_exploration_mode(!exploring);
            }
        }
    ));

    ih.set_long_press_handler("mr_navigation", std::function<void(int)>(
        [this](int key) { mr.handle_movement(key); }
    ));

    ih.set_mouse_handler("mr_navigation", std::function<void(double, double)>(
        [this](double xpos, double ypos) { mr.handle_movement(xpos, ypos); }
    ));

    ih.long_press_handler_enabled("mr_navigation", false);
    ih.mouse_handler_enabled("mr_navigation", false);
}

void Engine::run_loop() {
    while (!glfwWindowShouldClose(chassis.get_window())) {
        glClear(GL_COLOR_BUFFER_BIT);

        mr.render();
        if (!exploring) {
            gui.render();
        }

        glfwSwapBuffers(chassis.get_window());
        glfwPollEvents();
    }
}

void Engine::load_model(std::string const& path) {
    mr.load_model(path);
}

void Engine::set_exploration_mode(bool exploration_mode) {
    exploring = exploration_mode;
    
    if (exploration_mode) {
        glfwSetCursorPos(chassis.get_window(), 0, 0);
        glfwSetInputMode(chassis.get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        ih.long_press_handler_enabled("mr_navigation", true);
        ih.mouse_handler_enabled("mr_navigation", true);
    } else {
        glfwSetInputMode(chassis.get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);        

        ih.long_press_handler_enabled("mr_navigation", false);
        ih.mouse_handler_enabled("mr_navigation", false);
    }
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
