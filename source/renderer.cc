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

ModelRenderer::ModelRenderer()
    : Renderer{"res/shaders/model.vert", "res/shaders/model.frag"} {
    set_proj(sprogram.id());
}

void ModelRenderer::render() {
    glEnable(GL_DEPTH_TEST);
    glUseProgram(sprogram.id());

    for (auto& m : models) {
        m.draw();
    }
    
    glDisable(GL_DEPTH_TEST);
}

void ModelRenderer::load_model(std::string const& path) {
    std::string warn;
    std::string err;
    
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(path,
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

            // the docs don't mention anything, but I am gonna assume it's -1 in case it's missing
            models.emplace_back(attribs_data, indices_data);
        }
    } else {
        throw std::runtime_error("Failed to load model: " + path);
    }
}

void ModelRenderer::update_view(Camera& cam) {
    glUseProgram(sprogram.id());
    glm::mat4 view = cam.view();
    GLuint loc = glGetUniformLocation(sprogram.id(), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));
}

TexturedRenderer::TexturedRenderer()
    : Renderer{"res/shaders/textured.vert", "res/shaders/textured.frag"} {
    set_proj(sprogram.id());
}

void TexturedRenderer::render() {
    glEnable(GL_DEPTH_TEST);
    glUseProgram(sprogram.id());

    for (auto& m : models) {
        m.draw();
    }
    
    glDisable(GL_DEPTH_TEST);
}

void TexturedRenderer::load_model(std::string const& path) {
    std::string warn;
    std::string err;
    
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices
    );
    
    models.clear();
    if (scene) {
        std::vector<GLfloat> attribs_data;
        std::vector<GLuint> indices_data;

        for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
            const aiMesh* pMesh = scene->mMeshes[i];
            
            attribs_data.resize(pMesh->mNumVertices * 5);
            for (unsigned j = 0; j < pMesh->mNumVertices; ++j) {
                attribs_data[5*j+0] = pMesh->mVertices[j].x;
                attribs_data[5*j+1] = pMesh->mVertices[j].y;
                attribs_data[5*j+2] = pMesh->mVertices[j].z;
                
                attribs_data[5*j+3] = pMesh->mTextureCoords[0][j].x;
                attribs_data[5*j+4] = pMesh->mTextureCoords[0][j].y;
            }

            indices_data.resize(pMesh->mNumFaces * 3);
            for (unsigned j = 0; j < pMesh->mNumFaces; ++j) {
                indices_data[3*j+0] = pMesh->mFaces[j].mIndices[0];
                indices_data[3*j+1] = pMesh->mFaces[j].mIndices[1];
                indices_data[3*j+2] = pMesh->mFaces[j].mIndices[2];
            }

            // should check for pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0
            if (pMesh->mMaterialIndex > scene->mNumMaterials) {
                throw std::runtime_error("Some mesh doesn't have a material (texture)");
            }
            
            aiMaterial* pMaterial = scene->mMaterials[pMesh->mMaterialIndex];

            aiString path;
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
                models.emplace_back(attribs_data, indices_data, "res/models/" + std::string{path.data});
            }
        }
    } else {
        throw std::runtime_error("Failed to load model: " + path);
    }
}

void TexturedRenderer::update_view(Camera& cam) {
    glUseProgram(sprogram.id());
    glm::mat4 view = cam.view();
    GLuint loc = glGetUniformLocation(sprogram.id(), "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));
}
