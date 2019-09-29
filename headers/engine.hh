#pragma once

#define GLFW_INCLUDE_NONE
#include <cstdint>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "chassis.hh"
#include "shader.hh"
#include "renderer.hh"
#include "input_handler.hh"

class Engine {
private:

    class Gui {
    public:
        Gui(Engine& ngn);

        void render();

    private:
        Engine& ngn;

        std::string load_info;
        int selected = -1;
        
        void render_model_info();
    };
    
public:

    Engine(Chassis& chassis);

    void run_loop();
    void load_model(std::string const&);

private:

    Chassis& chassis;
    Camera cam;
    TexturedRenderer mr;
    Gui gui;
    InputHandler ih;

    bool exploring = false;
    double delta_time = 0.0;

    void set_exploration_mode(bool);

    static bool engine_started;
    static std::vector<std::string> model_names;

};
