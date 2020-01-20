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
    return glm::lookAt(pos, pos + forward, glm::vec3{0.f, 1.f, 0.f});
}

glm::vec3 Camera::to_world_space(glm::vec3 v) const {
    return glm::inverse(view()) * glm::vec4(v, 0.0);
}

float Frustum::get_orig_w(float pixel_depth) const {
    float proj_plane_d = 1.0f / glm::tan(glm::radians(fovy/2.0f));
    return pixel_depth / proj_plane_d;
}

glm::mat4 Frustum::projection() const {
    return glm::perspective(glm::radians(fovy), aspect_ratio, znear, zfar);
}
