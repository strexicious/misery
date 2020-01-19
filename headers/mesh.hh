#pragma once

#include <vector>
#include <cstddef>
#include <string>
#include <stdexcept>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

class Mesh {
public:

    Mesh(std::vector<GLfloat> const&, std::vector<GLuint> const&);
    
    Mesh(Mesh&&);
    virtual ~Mesh();

    virtual void draw();

    void translate(glm::vec3 dir, float units);
    glm::mat4 const& get_transform() const;
    glm::vec3 cur_translate() const;
    
protected:

    GLuint vao, vbo, ebo;
    glm::mat4 model_transform = glm::identity<glm::mat4>();
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
