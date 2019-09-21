#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

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

private:
    GLfloat move_speed = 0.1f;

    glm::vec3 forward{0.0, 0.0, -1.0};
    glm::vec3 pos{0.0};
};
