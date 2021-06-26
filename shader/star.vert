#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in float v_shimmer_offset;
layout (location = 3) in float v_theta;
layout (location = 4) in float v_size;

out vec3 g_color;
out float g_shimmer_offset;
out float g_theta;
out float g_size;

void main() {
    gl_Position = vec4(position, 0.0f, 1.0f);
    g_color = v_color;
    g_shimmer_offset = v_shimmer_offset;
    g_theta = v_theta;
    g_size = v_size;
}