#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#include "loadfont.h"

#define STELLQ_WINDOW_WIDTH 1500
#define STELLQ_WINDOW_HEIGHT 500

void GLAPIENTRY gl_message_callback(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei length,
                                    const GLchar *message,
                                    const void *userParam);

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
    SDL_GLContext  window_context;
    SDL_Event      event;

    if(init_window(&window, &window_context) == -1) return -1;

    TextRenderContext *text_context = load_text_render_context(font_path);

    if(text_context == NULL) return -1;

    load_text_to_context(text_context, text, 128);

    while(1) {
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                free_render_context(text_context);
                destroy_window(window, window_context);

                return 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        render_text_from_context(text_context);

        SDL_GL_SwapWindow(window);
    }
}

void GLAPIENTRY gl_message_callback(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei length,
                                    const GLchar *message,
                                    const void *userParam) {

    if(type == GL_DEBUG_TYPE_ERROR) {
        fprintf(stderr, "GL ERROR\n\tSEVERITY 0x%x\n\t%s\n", severity, message);
    }
}