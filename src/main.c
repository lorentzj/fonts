#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#include "loadfont.h"

void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

int init_window(SDL_Window **window, SDL_GLContext *context) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        printf("Failed to initialize SDL\n");
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,         SDL_GL_CONTEXT_DEBUG_FLAG);

    *window  = SDL_CreateWindow("StellQ", 0, 0, STELLQ_WINDOW_WIDTH, STELLQ_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    *context = SDL_GL_CreateContext(*window);

    if(glewInit() != 0) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_message_callback, 0);

    glViewport(0, 0, STELLQ_WINDOW_WIDTH, STELLQ_WINDOW_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    return 0;
}

int init_render(char* font_path, GLuint *shader_program, GLuint *font_texture) {
    if(load_font_shader_program(shader_program) == -1) return -1;
    if(load_font_texture(font_path, font_texture) == -1) return -1;

    return 0;
}

void release_render(GLuint shader_program, GLuint font_texture, GLuint vao, GLuint vbo, GLuint ebo) {
    glDeleteProgram(shader_program);
    glDeleteTextures(1, &font_texture);
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void destroy_window(SDL_Window *window, SDL_GLContext *context) {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage:\n\t%s [path/to/font.png] [string to display]\n", argv[0]);
        return -1;
    }

    char *font_path = argv[1];
    char *text      = argv[2];

    SDL_Window    *window;
    SDL_GLContext  context;
    SDL_Event      event;

    GLuint shader_program;
    GLuint font_texture;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    int text_length = strlen(text);

    if(init_window(&window, &context) == -1) return -1;
    if(init_render(font_path, &shader_program, &font_texture) == -1) return -1;

    RenderData render_data = load_vertices(text, text_length, &vao, &vbo, &ebo);

    while(1) {
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                release_render(shader_program, font_texture, vao, vbo, ebo);
                destroy_window(window, context);

                free(render_data.vertices);
                free(render_data.element_indices);
                
                return 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // 2 triangles per glyph, 3 vertices per triangle, so 6 vertices per glyph
        glDrawElements(GL_TRIANGLES, text_length*6, GL_UNSIGNED_INT, (void*)0);

        SDL_GL_SwapWindow(window);
    }
}

void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    if(type == GL_DEBUG_TYPE_ERROR) {
        fprintf(stderr, "GL ERROR\n\tSEVERITY 0x%x\n\t%s\n", severity, message);
    }
}