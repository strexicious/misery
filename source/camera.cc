#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include "camera.hh"

void Camera::move(Camera::MoveDir md) {
    pos += forward * move_speed * static_cast<float>(md);
}

void Camera::set_move_speed(float ms) {
    move_speed = ms;
}

void Camera::set_angles(float x, float y) {
    x *= move_speed;
    y *= move_speed;
    
    GLfloat cosX = glm::cos(glm::radians(x));
    GLfloat sinX = glm::sin(glm::radians(x));
    GLfloat cosY = glm::cos(glm::radians(y));
    GLfloat sinY = glm::sin(glm::radians(y));
    forward = glm::normalize(glm::vec3(sinX * cosY, -sinY, -cosX * cosY));
}

glm::mat4 Camera::view() const {
    // ugh, you can see it as a rotation and transformation if you wish, :stinky:
    // uses property of orthogonality
    
    glm::vec3 u_prime{0.0, 1.0, 0.0};
    glm::vec3 s = glm::cross(forward, u_prime);
    glm::vec3 up = glm::cross(s, forward);
    
    glm::mat4 res{
        glm::vec4{s.x, up.x, -forward.x, 0.0},
        glm::vec4{s.y, up.y, -forward.y, 0.0},
        glm::vec4{s.z, up.z, -forward.z, 0.0},
        glm::vec4{-glm::dot(s, pos), -glm::dot(up, pos), glm::dot(forward, pos), 1.0},
    };

    return res;
    // return glm::lookAt(pos, pos + forward, glm::vec3{0.f, 1.f, 0.f});
}

glm::vec3 Camera::to_world_space(glm::vec3 v) const {
    return glm::inverse(view()) * glm::vec4(v, 0.0);
}

float Frustum::get_orig_w(float pixel_depth) const {
    float proj_plane_d = 1.0f / glm::tan(glm::radians(fovy/2.0f));
    return pixel_depth / proj_plane_d;
}

glm::mat4 Frustum::projection() const {
    // the following matrix constructed is equivalent to glm's perspectiveRH_NO
    // for OpenGL NDC cuz it's in the name: *gl*m
    // Documentation from glm:
    /// Creates a matrix for a right handed, symetric perspective-view frustum.
	/// The near and far clip planes correspond to z normalized device coordinates of -1 and +1 respectively. (OpenGL clip volume definition)
    
    GLfloat proj_plane_d = 1 / glm::tan(glm::radians(fovy / 2));
    // custom perspective matrix to project into OpenGL's NDC from a
    // RHS camera looking towards -z axis system
    glm::mat4 res{
        glm::vec4{1.0/(aspect_ratio*proj_plane_d), 0.0, 0.0, 0.0},
        glm::vec4{0.0, 1.0/proj_plane_d, 0.0, 0.0},
        glm::vec4{0.0, 0.0, -(zfar+znear)/(zfar-znear), -1.0},
        glm::vec4{0.0, 0.0, -2*zfar*znear/(zfar-znear), 0.0},
    };
    
    return res;
    // return glm::perspective(glm::radians(fovy), aspect_ratio, znear, zfar);
}
