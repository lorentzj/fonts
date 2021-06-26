#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in float shimmer_offset;

uniform int u_time;

out vec3 f_color;

void main() {
    gl_Position = vec4(position, 0.0f, 1.0f);
    f_color = v_color * ((sin(u_time/1000.0f + shimmer_offset) * 2) - 1);
}