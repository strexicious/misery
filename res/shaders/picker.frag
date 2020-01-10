#version 330 core

out vec2 info_out;

uniform uint obj_id;

void main() {
    info_out = vec2(float(obj_id), float(gl_PrimitiveID + 1));
}
