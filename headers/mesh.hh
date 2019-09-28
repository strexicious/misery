#pragma once

#include <vector>
#include <cstddef>
#include <glad/glad.h>

class Mesh {
public:
    Mesh(std::vector<GLfloat> const&, std::vector<GLuint> const&);
    
    Mesh(Mesh&&);
    ~Mesh();

    void draw();
    
private:
    GLuint vao, vbo, ebo;
    std::size_t i_count;

};