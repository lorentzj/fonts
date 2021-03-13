#version 410 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 text_coord;

out vec2 f_text_coord;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    f_text_coord = text_coord;
}