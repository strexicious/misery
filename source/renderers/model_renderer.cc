#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "model_renderer.hh"

ModelRenderer::ModelRenderer(std::string const& vpath, std::string const& fpath)
    : sprogram{read_shader(vpath), read_shader(fpath)} {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);

    glm::mat4 view = glm::lookAt(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 0.f, -1.f}, glm::vec3{0.f, 1.f, 0.f});
    glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.77f, 1.f, 100.f);

    glUseProgram(sprogram.id());
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(proj));
}

void ModelRenderer::load_model(std::string const& path) {
    std::string warn;
    std::string err;
    
    if(tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        if (!warn.empty()) {
            std::cout << warn << std::endl;
        }

        if (!err.empty()) {
            std::cerr << err << std::endl;
        }
        
        std::vector<GLfloat> vertices;
        for (auto& i : shapes[0].mesh.indices) {
            vertices.push_back(attrib.vertices[3*i.vertex_index+0]);
            vertices.push_back(attrib.vertices[3*i.vertex_index+1]);
            vertices.push_back(attrib.vertices[3*i.vertex_index+2]);
        }
        v_count = vertices.size();
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3, vertices.data(), GL_STATIC_DRAW);
        loaded_path = path;
    } else {
        throw std::runtime_error("Failed to load model: " + path);
    }
}

void ModelRenderer::render() const {
    if (v_count) {
        glBindVertexArray(vao);
        glUseProgram(sprogram.id());
        glDrawArrays(GL_TRIANGLES, 0, v_count);
    }
}

std::string ModelRenderer::loaded_model() const {
    return loaded_path;
}
