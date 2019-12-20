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
    MeshRenderer<Mesh> cr{"res/shaders/model.vert", "res/shaders/model.frag"};
    MeshRenderer<TexturedMesh> tr{"res/shaders/textured.vert", "res/shaders/textured.frag"};
    Gui gui;
    InputHandler ih;

    bool exploring = false;
    double delta_time = 0.0;

    void set_exploration_mode(bool);
    void update_view();
    void compute_click_pixels();

    static bool engine_started;
    static std::vector<std::string> model_names;
    double explo_mx = 0.0;
    double explo_my = 0.0;
    double last_mxpos, last_mypos;

};
