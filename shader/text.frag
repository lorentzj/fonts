#version 330 core

out vec4 color;

in vec2 f_text_coord;
uniform sampler2D font;

void main() {
    color = vec4(1, 1, 1, texture(font, f_text_coord).a);
}