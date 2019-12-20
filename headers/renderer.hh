#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "mesh.hh"
#include "shader.hh"
#include "camera.hh"

class Renderer {
public:

    Renderer(std::string const&, std::string const&);

    virtual void render() = 0;

protected:

    Program sprogram;

};

template <typename T>
class MeshRenderer : public Renderer {
public:

    MeshRenderer(std::string const& vpath, std::string const& fpath);

    void render() override;
    void add_mesh(T m);
    void update_view(Camera&);

private:

    std::vector<T> models;
};

class PickerRenderer : public Renderer {
public:

    PickerRenderer(unsigned width, unsigned height);
    ~PickerRenderer();

    void render() override;
    unsigned get_mesh_id(unsigned x, unsigned y);

private:

    unsigned width, height;
    GLuint fb_obj;
    GLuint color_rbo, depth_rbo;

};

// ====================================================================
// :::::::::::::  MeshRenderer Templated Implementations  :::::::::::::
// ====================================================================

template<typename T>
MeshRenderer<T>::MeshRenderer(std::string const& vpath, std::string const& fpath)
    : Renderer{vpath, fpath} {
    glUseProgram(sprogram.id());
    glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.77f, 0.1f, 100.f);
    GLuint loc = glGetUniformLocation(sprogram.id(), "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(proj));
}

template<typename T>
void MeshRenderer<T>::render() {
    glEnable(GL_DEPTH_TEST);
    glUseProgram(sprogram.id());

    for (auto& m : models) {
        GLuint loc = glGetUniformLocation(sprogram.id(), "model");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m.get_transform()));

        m.draw();
    }
    
    glDisable(GL_DEPTH_TEST);
}

template<typename T>
void MeshRenderer<T>::add_mesh(T m) {
    models.push_back(std::move(m));
}

template<typename T>
void MeshRenderer<T>::update_view(Camera& cam) {
    glUseProgram(sprogram.id());
    glm::mat4 view = cam.view();
    GLuint loc = glGetUniformLocation(sprogram.id(), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));
}
