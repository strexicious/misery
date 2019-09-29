#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include "mesh.hh"

Mesh::Mesh(std::vector<GLfloat> const& attribs, std::vector<GLuint> const& indices)
    : i_count{indices.size()} {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, attribs.size() * sizeof(GLfloat), attribs.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

Mesh::Mesh(Mesh&& other)
    : vao{other.vao}, vbo{other.vbo}, ebo{other.ebo}, i_count{other.i_count} {
    other.vao = other.vbo = other.ebo = other.i_count = 0;
}

void Mesh::draw() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, i_count, GL_UNSIGNED_INT, (void*)(0));
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

TexturedMesh::TexturedMesh(
    std::vector<GLfloat> const& attribs,
    std::vector<GLuint> const& indices,
    std::string const& path
) : Mesh{attribs, indices} {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &comps, 0);

    if (!data) {
        throw std::runtime_error("Failed to load texture: " + path);
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

TexturedMesh::TexturedMesh(TexturedMesh&& other)
    : Mesh{std::move(other)}, texture{other.texture} {
    other.texture = 0;
}

void TexturedMesh::draw() {
    glBindTexture(GL_TEXTURE_2D, texture);
    Mesh::draw();
}

TexturedMesh::~TexturedMesh() {
    glDeleteTextures(1, &texture);
}
