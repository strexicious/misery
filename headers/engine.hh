#pragma once

#define GLFW_INCLUDE_NONE
#include <memory>
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

    struct MouseTrack {
        double cur_mx = 0.0;
        double cur_my = 0.0;
        double last_mxpos = 0.0;
        double last_mypos = 0.0;
    };
    
public:

    Engine(Chassis& chassis);

    void run_loop();
    void load_model(std::string const&);

private:

    Chassis& chassis;
    Camera cam;
    MeshRenderer cr{"res/shaders/model.vert", "res/shaders/model.frag"};
    MeshRenderer tr{"res/shaders/textured.vert", "res/shaders/textured.frag"};
    PickerRenderer pr;
    Gui gui;
    InputHandler ih;

    bool exploring = false;
    // we need to use picked to keep track if we just picked an object, to not immediately
    // drop it in the object_commt left button handler
    bool picked = false;
    std::optional<std::shared_ptr<Mesh>> moving_object = std::nullopt;
    glm::vec3 last_pos{0.0f, 0.0f, 0.0f};
    // we store the clicked fragment's depth from cam origin so we can use that to
    // calculate the homogenous coordinate w factor
    glm::float32 pixel_depth;
    double delta_time = 0.0;
    MouseTrack explo_pos;
    MouseTrack moving_obj_pos;
    std::vector<std::shared_ptr<Mesh>> p_models;

    void set_exploration_mode(bool);
    void update_view();

    static bool engine_started;
    static std::vector<std::string> model_names;

};
