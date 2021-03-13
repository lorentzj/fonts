#version 410 core

out vec4 color;

in vec2 f_tex_coord;
uniform sampler2D font;

void main() {
    color = texture(font, f_tex_coord);
}