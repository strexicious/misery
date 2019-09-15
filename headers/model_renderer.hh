#pragma once

#include <string>
#include <glad/glad.h>
#include <tiny_obj_loader.h>
#include "shader.hh"

class ModelRenderer {
public:
    ModelRenderer(std::string const& vpath, std::string const& fpath);

    void render() const;
    std::string loaded_model() const;

    void load_model(std::string const& path);

private:
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    Program sprogram;
    GLuint vao, vbo;
    std::size_t v_count = 0;

    std::string loaded_path;
};