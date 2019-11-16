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

// ===============================
// ::::::::::  UTILITY  ::::::::::
// ===============================

void set_proj(GLuint spid) {
    glUseProgram(spid);
    glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.77f, 0.1f, 100.f);
    GLuint loc = glGetUniformLocation(spid, "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(proj));
}

// =============================================
// ::::::::::  CLASS IMPLEMENTATIONS  ::::::::::
// =============================================

Renderer::Renderer(std::string const& vpath, std::string const& fpath)
    : sprogram{read_shader(vpath), read_shader(fpath)} { }

ColoredRenderer::ColoredRenderer()
    : MeshRenderer{"res/shaders/model.vert", "res/shaders/model.frag"} {
    set_proj(sprogram.id());
}

TexturedRenderer::TexturedRenderer()
    : MeshRenderer{"res/shaders/textured.vert", "res/shaders/textured.frag"} {
    set_proj(sprogram.id());
}
