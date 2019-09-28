#pragma once

#define GLFW_INCLUDE_NONE
#include <cstdint>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "chassis.hh"
#include "shader.hh"
#include "model_renderer.hh"
#include "input_handler.hh"

class Engine {
private:
    class Gui {
    public:
        Gui(Engine& ngn);

        void render();

    private:
        Engine& ngn;

        // Model
        char model_name_buf[32] = "Transport Shuttle_obj.obj";
        std::string load_info;
        
        void render_model_info();
    };
    
public:
    Engine(Chassis& chassis);

    void run_loop();
    void load_model(std::string const&);

private:
    Chassis& chassis;
    ModelRenderer mr;
    Gui gui;
    InputHandler ih;

    bool exploring = false;
    double delta_time = 0.0;

    void set_exploration_mode(bool);

    static bool engine_started;
};
