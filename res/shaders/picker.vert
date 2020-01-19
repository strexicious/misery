#version 330 core

in vec3 position;

out float pixel_depth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    pixel_depth = (view * model * vec4(position, 1.0)).z;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
