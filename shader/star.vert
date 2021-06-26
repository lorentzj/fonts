#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in float v_shimmer_offset;

out vec3 g_color;
out float g_shimmer_offset;

void main() {
    gl_Position = vec4(position, 0.0f, 1.0f);
    g_color = v_color;
    g_shimmer_offset = v_shimmer_offset;
}