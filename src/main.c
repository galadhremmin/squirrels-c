#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "utils/memcleanup.h"

// Cleanup functions for automatic resource management
static void cleanup_window(SDL_Window** window) {
    if (*window) {
        SDL_DestroyWindow(*window);
        *window = NULL;
    }
}

static void cleanup_renderer(SDL_Renderer** renderer) {
    if (*renderer) {
        SDL_DestroyRenderer(*renderer);
        *renderer = NULL;
    }
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window AUTO_CLEANUP_FUNC(cleanup_window) =
        SDL_CreateWindow("SDL Example", 640, 480, SDL_WINDOW_RESIZABLE);

    if (window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // cppcheck-suppress constVariablePointer
    // Pointer must be non-const: cleanup_renderer sets *renderer = NULL via
    // __attribute__((cleanup))
    SDL_Renderer* renderer AUTO_CLEANUP_FUNC(cleanup_renderer) = SDL_CreateRenderer(window, NULL);

    if (renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    game_run(window, renderer);

    SDL_Quit();
    return 0;
}
