#include <iostream>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "renderer.hh"

// =============================================
// ::::::::::  CLASS IMPLEMENTATIONS  ::::::::::
// =============================================

Renderer::Renderer(std::string const& vpath, std::string const& fpath)
    : sprogram{read_shader(vpath), read_shader(fpath)} {
    glUseProgram(sprogram.id());
    glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.77f, 0.1f, 100.f);
    GLuint loc = glGetUniformLocation(sprogram.id(), "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(proj));
}

void Renderer::update_view(Camera& cam) {
    glUseProgram(sprogram.id());
    glm::mat4 view = cam.view();
    GLuint loc = glGetUniformLocation(sprogram.id(), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));
}

void Renderer::active_default_fbo() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

MeshRenderer::MeshRenderer(std::string const& vpath, std::string const& fpath)
    : Renderer{vpath, fpath} { }

void MeshRenderer::render() {
    glEnable(GL_DEPTH_TEST);
    glUseProgram(sprogram.id());

    for (auto& p_m : p_models) {
        GLuint loc = glGetUniformLocation(sprogram.id(), "model");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(p_m->get_transform()));

        p_m->draw();
    }
    
    glDisable(GL_DEPTH_TEST);
}

void MeshRenderer::add_mesh(std::shared_ptr<Mesh> p_m) {
    p_models.push_back(std::move(p_m));
}

PickerRenderer::PickerRenderer(unsigned width, unsigned height)
    : Renderer{"res/shaders/picker.vert", "res/shaders/picker.frag"}, width{width}, height{height} {
    glGenFramebuffers(1, &fb_obj);

    glBindFramebuffer(GL_FRAMEBUFFER, fb_obj);

    glGenRenderbuffers(1, &color_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, color_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB32F, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rbo);
    
    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PickerRenderer::render() {
    glEnable(GL_DEPTH_TEST);
    glUseProgram(sprogram.id());

    for (std::size_t i = 0; i < p_models.size(); ++i) {
        GLuint loc = glGetUniformLocation(sprogram.id(), "model");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(p_models[i]->get_transform()));
        loc = glGetUniformLocation(sprogram.id(), "obj_id");
        glUniform1uiv(loc, 1, (GLuint const*) &i);

        p_models[i]->draw();
    }
    
    glDisable(GL_DEPTH_TEST);
}

void PickerRenderer::add_mesh(std::shared_ptr<Mesh> p_m) {
    p_models.push_back(std::move(p_m));
}

void PickerRenderer::active_fbo() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_obj);
}

std::optional<std::pair<unsigned, glm::float32>> PickerRenderer::get_mesh_info(unsigned x, unsigned y) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_obj);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    GLfloat obj_id = 0.0f;
    glReadPixels(x, height - y, 1, 1, GL_RED, GL_FLOAT, &obj_id);

    GLfloat prim_id = 0.0f;
    glReadPixels(x, height - y, 1, 1, GL_GREEN, GL_FLOAT, &prim_id);

    glm::float32 pixel_depth;
    glReadPixels(x, height - y, 1, 1, GL_BLUE, GL_FLOAT, &pixel_depth);
    
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    if (prim_id != 0.0f) {
        return std::make_pair(static_cast<unsigned>(obj_id), pixel_depth);
    } else {
        return std::nullopt;
    }
}

PickerRenderer::~PickerRenderer() {
    glDeleteFramebuffers(1, &fb_obj);
    glDeleteRenderbuffers(1, &color_rbo);
    glDeleteRenderbuffers(1, &depth_rbo);
}
