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
    void update_projection(Frustum& fru);
    void update_view(Camera&);

    void active_default_fbo();

protected:

    Program sprogram;

};

class MeshRenderer : public Renderer {
public:

    MeshRenderer(std::string const& vpath, std::string const& fpath, std::vector<std::unique_ptr<Mesh>>& models);

    void render() override;
    void add_mesh_idx(std::size_t i);

private:

    std::vector<std::unique_ptr<Mesh>>& models;
    std::vector<std::size_t> models_idx;
};

class PickerRenderer : public Renderer {
public:

    PickerRenderer(unsigned width, unsigned height, std::vector<std::unique_ptr<Mesh>>& models);
    ~PickerRenderer();

    void render() override;
    void add_mesh_idx(std::size_t i);
    
    void active_fbo();
    
    std::optional<std::pair<unsigned, float>> get_mesh_info(unsigned x, unsigned y);

private:

    unsigned width, height;
    GLuint fb_obj;
    GLuint color_rbo, depth_rbo;

    std::vector<std::unique_ptr<Mesh>>& models;
    std::vector<std::size_t> models_idx;
    
};
