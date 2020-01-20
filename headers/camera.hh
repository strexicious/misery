#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera {
public:

    enum class MoveDir : int {
        backward = -1,
        forward = 1,
    };
    
public:

    void move(MoveDir md);
    void set_move_speed(GLfloat ms);
    void set_angles(GLfloat x, GLfloat y);
    
    glm::mat4 view() const;
    glm::vec3 to_world_space(glm::vec3 v) const;

private:

    float move_speed = 0.1f;

    glm::vec3 forward{0.0f, 0.0f, -1.0f};
    glm::vec3 pos{0.0f, 0.0f, 5.0f};

};

class Frustum {
public:

    Frustum(unsigned width, unsigned height)
        : aspect_ratio{width / static_cast<float>(height)} { }
    
    float get_orig_w(float pixel_depth) const;
    glm::mat4 projection() const;

    float get_aspect_ratio() const { return aspect_ratio; }

private:

    float fovy = 90.0f;
    float znear = 0.1f;
    float zfar = 100.0f;
    float aspect_ratio;

};
