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
    : sprogram{read_shader(vpath), read_shader(fpath)} { }

PickerRenderer::PickerRenderer(unsigned width, unsigned height)
    : Renderer{"res/shaders/picker.vert", "res/shaders/picker.frag"}, width{width}, height{height} {
    glGenFramebuffers(1, &fb_obj);

    glBindFramebuffer(GL_FRAMEBUFFER, fb_obj);

    glGenRenderbuffers(1, &color_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, color_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_R3_G3_B2, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rbo);
    
    glGenRenderbuffers(1, &depth_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_R3_G3_B2, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PickerRenderer::render() {
    
}

unsigned PickerRenderer::get_mesh_id(unsigned x, unsigned y) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_obj);

    unsigned id;
    glReadPixels(x, height - y, 1, 1, GL_RGB, GL_UNSIGNED_INT, &id);
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return id;
}

PickerRenderer::~PickerRenderer() {
    glDeleteFramebuffers(1, &fb_obj);
    glDeleteRenderbuffers(1, &color_rbo);
    glDeleteRenderbuffers(1, &depth_rbo);
}
