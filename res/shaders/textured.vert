#version 330 core

in vec3 position;
in vec2 texcoords;

out vec2 f_texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    f_texcoords = texcoords;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
