#include <stdio.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 500

int init(SDL_Window **window, SDL_GLContext *context) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) return -1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,         SDL_GL_CONTEXT_DEBUG_FLAG);

    *window  = SDL_CreateWindow("StellQ", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    *context = SDL_GL_CreateContext(*window);

    if(glewInit() != 0) return -1;

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_BLEND);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    return 0;
}

void destroy(SDL_Window *window, SDL_GLContext *context) {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    SDL_Window    *window  = NULL;
    SDL_GLContext  context = NULL;
    SDL_Event      event;

    if(init(&window, &context) != 0) return -1;

    while(1) {
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                destroy(window, context);
                return 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }
}