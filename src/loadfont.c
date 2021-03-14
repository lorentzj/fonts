#include <GL/glew.h>
#include <SDL2/SDL_image.h>

#include "loadfont.h"

GLuint compile_shader_from_path(const char* path, GLuint shader_type);

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

int load_font_texture(const char* texture_path, GLuint *texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    

    if(!IMG_Init(IMG_INIT_PNG)) {
        printf("Failed to initialize SDL_image\n");
        return -1;
    }

    SDL_Surface* image = IMG_Load(texture_path);
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

RenderData load_vertices(const char* text, int text_length, GLuint *vao, GLuint *vbo, GLuint *ebo) {
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
    
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(sizeof(float)*2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    size_t v_buffer_size = text_length * 4 * sizeof(TextVertex);

    TextVertex* vertex_data = malloc(v_buffer_size);

    float center = (float)text_length/2*STELLQ_CHAR_WIDTH/STELLQ_WINDOW_WIDTH;

    for(int i = 0; i < text_length; ++i) {
        vertex_data[i*4 + 0].px = (float)i     *STELLQ_CHAR_WIDTH/STELLQ_WINDOW_WIDTH - center;
        vertex_data[i*4 + 1].px = (float)i     *STELLQ_CHAR_WIDTH/STELLQ_WINDOW_WIDTH - center;
        vertex_data[i*4 + 2].px = (float)(i+1) *STELLQ_CHAR_WIDTH/STELLQ_WINDOW_WIDTH - center;
        vertex_data[i*4 + 3].px = (float)(i+1) *STELLQ_CHAR_WIDTH/STELLQ_WINDOW_WIDTH - center;

        vertex_data[i*4 + 0].py = 0;
        vertex_data[i*4 + 1].py = (float)STELLQ_CHAR_HEIGHT/STELLQ_WINDOW_HEIGHT;
        vertex_data[i*4 + 2].py = 0;
        vertex_data[i*4 + 3].py = (float)STELLQ_CHAR_HEIGHT/STELLQ_WINDOW_HEIGHT;

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

    uint32_t* element_data = malloc(e_buffer_size);

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

    // for(int i = 0; i < text_length; ++i) {
    //     printf("(%f %f) (%f %f)\n", vertex_data[i*6 + 0].px, vertex_data[i*6 + 0].py, vertex_data[i*6 + 0].tx, vertex_data[i*6 + 0].ty);
    //     printf("(%f %f) (%f %f)\n", vertex_data[i*6 + 1].px, vertex_data[i*6 + 1].py, vertex_data[i*6 + 1].tx, vertex_data[i*6 + 1].ty);
    //     printf("(%f %f) (%f %f)\n", vertex_data[i*6 + 2].px, vertex_data[i*6 + 2].py, vertex_data[i*6 + 2].tx, vertex_data[i*6 + 2].ty);
    //     printf("(%f %f) (%f %f)\n", vertex_data[i*6 + 3].px, vertex_data[i*6 + 3].py, vertex_data[i*6 + 3].tx, vertex_data[i*6 + 3].ty);
    //     printf("(%f %f) (%f %f)\n", vertex_data[i*6 + 4].px, vertex_data[i*6 + 4].py, vertex_data[i*6 + 4].tx, vertex_data[i*6 + 4].ty);
    //     printf("(%f %f) (%f %f)\n", vertex_data[i*6 + 5].px, vertex_data[i*6 + 5].py, vertex_data[i*6 + 5].tx, vertex_data[i*6 + 5].ty);
    // }

    RenderData rdata;
    rdata.element_indices = element_data;
    rdata.vertices = vertex_data;

    return rdata;
}

GLuint compile_shader_from_path(const char* path, GLuint shader_type) {
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