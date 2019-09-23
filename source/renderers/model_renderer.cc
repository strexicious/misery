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

    update_view();
    update_proj();
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
        v_count = vertices.size() / 3;
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    } else {
        throw std::runtime_error("Failed to load model: " + path);
    }
}

void ModelRenderer::update_view() {
    glUseProgram(sprogram.id());
    glm::mat4 view = cam.view();
    GLuint loc = glGetUniformLocation(sprogram.id(), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));
}

void ModelRenderer::update_proj() {
    glUseProgram(sprogram.id());
    glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.77f, 0.1f, 100.f);
    GLuint loc = glGetUniformLocation(sprogram.id(), "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(proj));
}

void ModelRenderer::render() const {
    if (v_count) {
        glEnable(GL_DEPTH_TEST);
        
        glBindVertexArray(vao);
        glUseProgram(sprogram.id());
        glDrawArrays(GL_TRIANGLES, 0, v_count);

        glDisable(GL_DEPTH_TEST);
    }
}

void ModelRenderer::handle_movement(int key) {
    if (key == GLFW_KEY_W) {
        cam.move(Camera::MoveDir::forward);
        update_view();
    }

    if (key == GLFW_KEY_S) {
        cam.move(Camera::MoveDir::backward);
        update_view();
    }
}

void ModelRenderer::handle_movement(double xpos, double ypos) {
    cam.set_angles(xpos, ypos);
    update_view();
}

ModelRenderer::~ModelRenderer() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
