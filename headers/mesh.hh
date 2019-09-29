#pragma once

#include <vector>
#include <cstddef>
#include <string>
#include <stdexcept>
#include <glad/glad.h>

class Mesh {
public:
    Mesh(std::vector<GLfloat> const&, std::vector<GLuint> const&);
    
    Mesh(Mesh&&);
    virtual ~Mesh();

    virtual void draw();
    
protected:
    GLuint vao, vbo, ebo;
    std::size_t i_count;

};

class TexturedMesh : public Mesh {
public:
    TexturedMesh(std::vector<GLfloat> const&, std::vector<GLuint> const&, std::string const&);

    TexturedMesh(TexturedMesh&&);
    ~TexturedMesh() override;

    void draw() override;

private:
    GLuint texture;
    int width, height, comps;

};
