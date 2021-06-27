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
    float theta;
    float size;
} StarData;

typedef struct star_render_context {
    GLuint shader_program;
    GLint u_time_location;
    GLint u_aspect_location;
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
        context->stars[i].b = random_between_0_and_1() + 0.5;

        context->stars[i].shimmer_offset = random_between_0_and_1() * 3.14159 * 2;
        context->stars[i].theta = random_between_0_and_1() * 3.14159 * 2;
        context->stars[i].size = random_between_0_and_1() * 0.002 + 0.001;
    }

    if(load_star_shader_program(&context->shader_program, context) == -1) return NULL;

    int viewport_data[4];
    glGetIntegerv(GL_VIEWPORT, viewport_data);
    int window_width  = viewport_data[2];
    int window_height = viewport_data[3];

    glUniform1f(context->u_aspect_location, window_height/(float)window_width);

    glGenVertexArrays(1, &context->vao);
    glBindVertexArray(context->vao);

    glGenBuffers(1, &context->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, context->vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(StarData), (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StarData), (void*)(sizeof(float)*2));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(StarData), (void*)(sizeof(float)*5));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(StarData), (void*)(sizeof(float)*6));
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(StarData), (void*)(sizeof(float)*7));
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

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
    GLuint gshader = compile_shader_from_path("shader/star.geom", GL_GEOMETRY_SHADER);
    GLuint fshader = compile_shader_from_path("shader/star.frag", GL_FRAGMENT_SHADER);

    if(vshader == -1 || gshader == -1 || fshader == -1) return -1;

    // Link Shader Program
    *program = glCreateProgram();

    glAttachShader(*program, vshader);
    glAttachShader(*program, gshader);
    glAttachShader(*program, fshader);

    glLinkProgram(*program);

    glDeleteShader(vshader);
    glDeleteShader(gshader);
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
    context->u_aspect_location = glGetUniformLocation(*program, "u_aspect");

    return 0;
}

void free_star_render_context(StarRenderContext *context) {
    glDeleteProgram(context->shader_program);
    free(context->stars);
    free(context);
}