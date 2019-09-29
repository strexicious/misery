#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
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

class ModelRenderer : public Renderer {
public:

    ModelRenderer();

    void render() override;
    void load_model(std::string const&);
    void update_view(Camera&);

private:

    std::vector<Mesh> models;
    
};

class TexturedRenderer : public Renderer {
public:

    TexturedRenderer();

    void render() override;
    void load_model(std::string const&);
    void update_view(Camera&);

private:

    std::vector<TexturedMesh> models;

    aiScene const* scene = nullptr;

};
