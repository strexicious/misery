#version 330 core

in vec2 f_texcoords;

out vec4 color;

uniform sampler2D tex_sampler;

void main() {
    color = texture(tex_sampler, f_texcoords);
}
