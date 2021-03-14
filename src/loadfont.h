#ifndef LOAD_FONT_H
#define LOAD_FONT_H

#define STELLQ_WINDOW_WIDTH 1500
#define STELLQ_WINDOW_HEIGHT 500
#define STELLQ_WINDOW_ASPECT ((float)STELLQ_WINDOW_HEIGHT/STELLQ_WINDOW_WIDTH)
#define STELLQ_CHAR_HEIGHT 128
#define STELLQ_CHAR_WIDTH (STELLQ_CHAR_HEIGHT/2)

typedef struct text_vertex {
    float px;
    float py;
    float tx;
    float ty;
} TextVertex;

typedef struct render_data {
    TextVertex* vertices;
    uint32_t* element_indices;
} RenderData;

int load_font_shader_program(GLuint *program);
int load_font_texture(const char* texture_path, GLuint *texture);
RenderData load_vertices(char* text, int text_length, GLuint *vao, GLuint *vbo, GLuint *ebo);

#endif