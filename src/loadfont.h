#ifndef LOAD_FONT_H
#define LOAD_FONT_H

typedef struct text_vertex {
    float px;
    float py;
    float tx;
    float ty;
} TextVertex;

int load_font_shader_program(GLuint *program);
int load_font_texture(const char* texture_path, GLuint *texture);
TextVertex* load_vertices(const char* text, int text_length, GLuint *vao, GLuint *vbo);

#endif