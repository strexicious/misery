#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "model_renderer.hh"

ModelRenderer::ModelRenderer(std::string const& vpath, std::string const& fpath)
    : sprogram{read_shader(vpath), read_shader(fpath)} {
    update_view();
    update_proj();
}

void ModelRenderer::load_model(std::string const& path) {
    std::string warn;
    std::string err;
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices
    );
    
    models.clear();
    if (scene) {
        std::vector<GLfloat> attribs_data;
        std::vector<GLuint> indices_data;

        for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
            const aiMesh* pMesh = scene->mMeshes[i];
            
            attribs_data.resize(pMesh->mNumVertices * 3);
            for (unsigned j = 0; j < pMesh->mNumVertices; ++j) {
                attribs_data[3*j+0] = pMesh->mVertices[j].x;
                attribs_data[3*j+1] = pMesh->mVertices[j].y;
                attribs_data[3*j+2] = pMesh->mVertices[j].z;
            }

            indices_data.resize(pMesh->mNumFaces * 3);
            for (unsigned j = 0; j < pMesh->mNumFaces; ++j) {
                indices_data[3*j+0] = pMesh->mFaces[j].mIndices[0];
                indices_data[3*j+1] = pMesh->mFaces[j].mIndices[1];
                indices_data[3*j+2] = pMesh->mFaces[j].mIndices[2];
            }

            models.emplace_back(attribs_data, indices_data);
        }
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

void ModelRenderer::render() {
    glEnable(GL_DEPTH_TEST);
    glUseProgram(sprogram.id());

    for (auto& m : models) {
        m.draw();
    }
    
    glDisable(GL_DEPTH_TEST);
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
