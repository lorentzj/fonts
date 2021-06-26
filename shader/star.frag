#version 330 core

in vec3 f_color;
in float f_shimmer_offset;

out vec4 color;

uniform int u_time;

void main() {
    float shimmer = (sin(u_time/1000.0f + f_shimmer_offset) * 2) - 1;
    color = vec4(f_color * shimmer, shimmer);
}