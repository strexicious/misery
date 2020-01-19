#pragma once

#include <memory>
#include <optional>
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
    void update_view(Camera&);

    void active_default_fbo();

protected:

    Program sprogram;

};

class MeshRenderer : public Renderer {
public:

    MeshRenderer(std::string const& vpath, std::string const& fpath);

    void render() override;
    void add_mesh(std::shared_ptr<Mesh> p_m);

private:

    std::vector<std::shared_ptr<Mesh>> p_models;
};

class PickerRenderer : public Renderer {
public:

    PickerRenderer(unsigned width, unsigned height);
    ~PickerRenderer();

    void render() override;
    void add_mesh(std::shared_ptr<Mesh> p_m);
    
    void active_fbo();
    
    std::optional<std::pair<unsigned, glm::float32>> get_mesh_info(unsigned x, unsigned y);

private:

    unsigned width, height;
    GLuint fb_obj;
    GLuint color_rbo, depth_rbo;

    std::vector<std::shared_ptr<Mesh>> p_models;
    
};
