#version 330 core

in vec3 position;

out float f_height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    f_height = (position.y + 5.0) / 10.0;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
