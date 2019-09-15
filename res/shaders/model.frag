#version 150 core

in float f_height;

out vec4 color;

void main() {
    color = vec4(0.6, 0.2, 0.3, 1.0) * f_height;
}
