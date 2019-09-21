#pragma once

#include <string>
#include <glad/glad.h>
#include <tiny_obj_loader.h>
#include <GLFW/glfw3.h>
#include "shader.hh"
#include "camera.hh"

class ModelRenderer {
public:
    ModelRenderer(std::string const& vpath, std::string const& fpath);

    void render() const;

    void load_model(std::string const& path);
    void update_view();
    
    static void handle_key_input(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void handle_mouse_input(GLFWwindow* window, double xpos, double ypos);

private:
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    Program sprogram;
    GLuint vao, vbo;
    std::size_t v_count = 0;

    Camera cam;
};
