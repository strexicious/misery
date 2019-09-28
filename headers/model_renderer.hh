#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "mesh.hh"
#include "shader.hh"
#include "camera.hh"

class ModelRenderer {
public:
    ModelRenderer(std::string const& vpath, std::string const& fpath);

    void render();
    void load_model(std::string const& path);
    void update_view();
    void update_proj();
    
    void handle_movement(int key); // keyboard
    void handle_movement(double xpos, double ypos); // mouse

private:
    Camera cam;
    std::vector<Mesh> models;
    Program sprogram;
};
