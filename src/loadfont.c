#include <GL/glew.h>
#include <SDL2/SDL_image.h>

#include "loadfont.h"

typedef struct text_vertex {
    float px;
    float py;
    float tx;
    float ty;
} TextVertex;

typedef struct text_render_data {
    TextVertex* vertices;
    uint32_t* element_indices;
} TextRenderData;

typedef struct text_render_context {
    GLuint shader_program;
    GLuint texture;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    TextRenderData data;
    int text_length;
    int text_loaded;
} TextRenderContext;

GLuint compile_shader_from_path(const char *path, GLuint shader_type);
int load_font_shader_program(GLuint *program);
int load_font_texture(char *texture_path, GLuint *texture);
TextRenderData load_vertices(char *text, int text_length, GLuint *vao, GLuint *vbo, GLuint *ebo, int text_height);

TextRenderContext* load_text_render_context(char *font_path) {
    TextRenderContext *context = malloc(sizeof(TextRenderContext));

    if(load_font_shader_program(&context->shader_program) == -1) return NULL;
    if(load_font_texture(font_path, &context->texture) == -1) return NULL;

    context->vao = -1;
    context->vbo = -1;
    context->ebo = -1;
    context->text_length = -1;
    context->text_loaded = 0;
    context->data.vertices = NULL;
    context->data.element_indices = NULL;
    return context;
}

void load_text_to_context(TextRenderContext *context, char *text, int text_height) {
    if(context->text_loaded) {
        glDeleteBuffers(1, &context->vao);
        glDeleteBuffers(1, &context->vbo);
        glDeleteBuffers(1, &context->ebo);
        free(context->data.vertices);
        free(context->data.element_indices);
    }

    context->text_length = strlen(text);
    context->data = load_vertices(text, context->text_length, &context->vao, &context->vbo, &context->ebo, text_height);
    context->text_loaded = 1;
}

void render_text_from_context(TextRenderContext *context) {
    glUseProgram(context->shader_program);

    // 2 triangles per glyph, 3 vertices per triangle, so 6 vertices per glyph
    glDrawElements(GL_TRIANGLES, context->text_length*6, GL_UNSIGNED_INT, (void*)0);
}

void free_render_context(TextRenderContext *context) {
    glDeleteProgram(context->shader_program);
    glDeleteTextures(1, &context->texture);

    if(context->text_loaded) {
        glDeleteBuffers(1, &context->vao);
        glDeleteBuffers(1, &context->vbo);
        glDeleteBuffers(1, &context->ebo);

        free(context->data.vertices);
        free(context->data.element_indices);
    }

    free(context);
}

int load_font_shader_program(GLuint *program) {
    int ok;
    char shader_error[GL_INFO_LOG_LENGTH];

    GLuint vshader = compile_shader_from_path("shader/text.vert", GL_VERTEX_SHADER);
    GLuint fshader = compile_shader_from_path("shader/text.frag", GL_FRAGMENT_SHADER);
    if(vshader == -1 || fshader == -1) return -1;

    // Link Shader Program
    *program = glCreateProgram();

    glAttachShader(*program, vshader);
    glAttachShader(*program, fshader);

    glLinkProgram(*program);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    glGetProgramiv(*program, GL_LINK_STATUS, &ok);

    if(!ok) {
        glGetProgramInfoLog(*program, GL_INFO_LOG_LENGTH, NULL, shader_error);
    	glDeleteProgram(*program);
        printf("Error Linking Shader Program:\n%s\n", shader_error);
        return -1;
    }

    glUseProgram(*program);

    return 0;
}

int load_font_texture(char *texture_path, GLuint *texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    

    if(!IMG_Init(IMG_INIT_PNG)) {
        printf("Failed to initialize SDL_image\n");
        return -1;
    }

    SDL_Surface *image = IMG_Load(texture_path);
    if(image == NULL) {
        printf("Error loading font texture: \n%s\n", IMG_GetError());        
        return -1;
    }

    if(image->w != 1024 || image->h != 16) {
        printf("Error loading font at %s\nImage size should be 1024x16px; found %dx%dpx.\n", texture_path, image->w, image->h);
        return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

    SDL_FreeSurface(image);

    return 0;
}

TextRenderData load_vertices(char *text, int text_length, GLuint *vao, GLuint *vbo, GLuint *ebo, int text_height) {
    int text_width = text_height/2;
    
    int viewport_data[4];
    glGetIntegerv(GL_VIEWPORT, viewport_data);
    int window_width = viewport_data[2];
    int window_height = viewport_data[3];

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
    
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(sizeof(float)*2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    size_t v_buffer_size = text_length * 4 * sizeof(TextVertex);

    TextVertex *vertex_data = malloc(v_buffer_size);

    float center = (float)text_length/2*text_width/window_width;

    for(int i = 0; i < text_length; ++i) {
        vertex_data[i*4 + 0].px = (float)i    *text_width/window_width - center;
        vertex_data[i*4 + 1].px = (float)i    *text_width/window_width - center;
        vertex_data[i*4 + 2].px = (float)(i+1)*text_width/window_width - center;
        vertex_data[i*4 + 3].px = (float)(i+1)*text_width/window_width - center;

        vertex_data[i*4 + 0].py = 0;
        vertex_data[i*4 + 1].py = (float)text_height/window_height;
        vertex_data[i*4 + 2].py = 0;
        vertex_data[i*4 + 3].py = (float)text_height/window_height;

        vertex_data[i*4 + 0].tx = (float)text[i]/128;
        vertex_data[i*4 + 1].tx = (float)text[i]/128;
        vertex_data[i*4 + 2].tx = (float)(text[i] + 1)/128;
        vertex_data[i*4 + 3].tx = (float)(text[i] + 1)/128;

        vertex_data[i*4 + 0].ty = 1;
        vertex_data[i*4 + 1].ty = 0;
        vertex_data[i*4 + 2].ty = 1;
        vertex_data[i*4 + 3].ty = 0;
    }

    glBufferData(GL_ARRAY_BUFFER, v_buffer_size, vertex_data, GL_STATIC_DRAW);
    
    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);

    size_t e_buffer_size = text_length * 6 * sizeof(uint32_t);

    uint32_t *element_data = malloc(e_buffer_size);

    for(int i = 0; i < text_length; ++i) {
        /**
         * [2,3].........[5] 
         * .    .        .
         * .        .    .
         * .            ..
         * [0]...........[1,4]
         */

        element_data[i*6 + 0] = i*4 + 0;
        element_data[i*6 + 1] = i*4 + 2;
        element_data[i*6 + 2] = i*4 + 1;
        element_data[i*6 + 3] = i*4 + 1;
        element_data[i*6 + 4] = i*4 + 2;
        element_data[i*6 + 5] = i*4 + 3;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, e_buffer_size, element_data, GL_STATIC_DRAW);

    TextRenderData rdata;
    rdata.element_indices = element_data;
    rdata.vertices = vertex_data;

    return rdata;
}

GLuint compile_shader_from_path(const char *path, GLuint shader_type) {
    int ok;
    int file_length;
    char shader_error[GL_INFO_LOG_LENGTH];

    // Load Shader Source
    FILE *shader_handle = fopen(path, "r");
    if(shader_handle == NULL) {
        printf("Failed to open %s\n", path);
        return -1;
    }

    fseek(shader_handle, 0, SEEK_END);
    file_length = ftell(shader_handle);
    rewind(shader_handle);
    char *shader_source = malloc(file_length + 1);
    if(shader_source == NULL) {
        fclose(shader_handle);
        printf("Failed to allocate space for contents of %s\n", path);
        return -1;
    }

    fread(shader_source, file_length + 1, 1, shader_handle);
    shader_source[file_length] = '\0';
    fclose(shader_handle);
    
    // Compile Shader
    GLuint shader = glCreateShader(shader_type);

    const char *shader_source_const = (const char*)shader_source;
    glShaderSource(shader, 1, &shader_source_const, NULL);
    glCompileShader(shader);
    free(shader_source);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if(!ok) {
        glGetShaderInfoLog(shader, GL_INFO_LOG_LENGTH, NULL, shader_error);
        glDeleteShader(shader);
        printf("Error Compiling Shader at %s:\n%s\n", path, shader_error);
        return -1;
    }

    return shader;
}