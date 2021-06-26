#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

#include "star_bg.h"
#include "compile_shader_from_path.h"

float random_between_0_and_1();
int load_star_shader_program(GLuint* program, StarRenderContext* context);

typedef struct star_data {
    float px;
    float py;
    float r;
    float g;
    float b;
    float shimmer_offset;
} StarData;

typedef struct star_render_context {
    GLuint shader_program;
    GLint u_time_location;
    GLuint vao;
    GLuint vbo;
    StarData* stars;
    int n_stars;
} StarRenderContext;

StarRenderContext* load_star_render_context(int n_stars) {
    srand(0);
    StarRenderContext* context = malloc(sizeof(StarRenderContext));

    context->n_stars = n_stars;
    context->stars = malloc(sizeof(StarData) * n_stars);

    for(int i = 0; i < n_stars; ++i) {
        context->stars[i].px = random_between_0_and_1() * 2.0 - 1.0;
        context->stars[i].py = random_between_0_and_1() * 2.0 - 1.0;

        context->stars[i].r = random_between_0_and_1();
        context->stars[i].g = random_between_0_and_1();
        context->stars[i].b = random_between_0_and_1();

        context->stars[i].shimmer_offset = random_between_0_and_1() * 3.14159 * 2;
    }

    if(load_star_shader_program(&context->shader_program, context) == -1) return NULL;

    glGenVertexArrays(1, &context->vao);
    glBindVertexArray(context->vao);

    glGenBuffers(1, &context->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, context->vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(StarData), (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StarData), (void*)(sizeof(float)*2));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(StarData), (void*)(sizeof(float)*5));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBufferData(GL_ARRAY_BUFFER, sizeof(StarData) * n_stars, context->stars, GL_STATIC_DRAW);

    return context;
}

void render_star_background(StarRenderContext *context, int program_time) {
    glUseProgram(context->shader_program);
    glBindVertexArray(context->vao);

    glUniform1i(context->u_time_location, program_time);

    glDrawArrays(GL_POINTS, 0, context->n_stars);
}

float random_between_0_and_1() {
    return rand()/(float)RAND_MAX;
}

int load_star_shader_program(GLuint* program, StarRenderContext* context) {
    int ok;
    char shader_error[GL_INFO_LOG_LENGTH];

    GLuint vshader = compile_shader_from_path("shader/star.vert", GL_VERTEX_SHADER);
    GLuint fshader = compile_shader_from_path("shader/star.frag", GL_FRAGMENT_SHADER);
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
        printf("Error Linking Star Shader Program:\n%s\n", shader_error);
        return -1;
    }

    glUseProgram(*program);

    context->u_time_location = glGetUniformLocation(*program, "u_time");

    return 0;
}

void free_star_render_context(StarRenderContext *context) {
    glDeleteProgram(context->shader_program);
    free(context->stars);
    free(context);
}