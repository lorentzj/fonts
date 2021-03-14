#version 330 core

out vec4 color;

in vec2 f_text_coord;
uniform sampler2D font;

void main() {
    color = texture(font, f_text_coord);
}