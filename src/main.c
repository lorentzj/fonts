#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#define STELLQ_WINDOW_WIDTH 1000
#define STELLQ_WINDOW_HEIGHT 500

#include "loadfont.h"

int init_window(SDL_Window **window, SDL_GLContext *context) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) return -1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,         SDL_GL_CONTEXT_DEBUG_FLAG);

    *window  = SDL_CreateWindow("StellQ", 0, 0, STELLQ_WINDOW_WIDTH, STELLQ_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    *context = SDL_GL_CreateContext(*window);

    if(glewInit() != 0) return -1;

    glViewport(0, 0, STELLQ_WINDOW_WIDTH, STELLQ_WINDOW_HEIGHT);
    glEnable(GL_BLEND);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    if(load_font_shader_program() == -1) return -1;

    return 0;
}

void destroy_window(SDL_Window *window, SDL_GLContext *context) {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    SDL_Window    *window  = NULL;
    SDL_GLContext  context = NULL;
    SDL_Event      event;

    if(init_window(&window, &context) != 0) return -1;

    while(1) {
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                destroy_window(window, context);
                return 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }
}