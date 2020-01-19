#version 330 core

in float pixel_depth;

out vec3 info_out;

uniform uint obj_id;

void main() {
    info_out = vec3(float(obj_id), float(gl_PrimitiveID + 1), pixel_depth);
}
