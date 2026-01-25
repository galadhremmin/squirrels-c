#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/memcleanup.h"
#include "utils/timer.h"
#include "world/render.h"
#include "world/world.h"

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

    World* world AUTO_CLEANUP_FUNC(world_free) = world_new(renderer);

    world_render_init(world);

    Timer timer = {
        .current_time = SDL_GetTicksNS(),
        .delta_time = 0,
    };

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT:
                quit = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (e.key.scancode == SDL_SCANCODE_ESCAPE) {
                    quit = true;
                    break;
                }
                __attribute__((fallthrough));
            default:
                world_process_input(world, &e);
                break;
            }
        }

        uint64_t current_time = SDL_GetTicksNS();
        uint64_t time_diff = current_time - timer.current_time;
        timer.delta_time = (float)time_diff / 1000000000.0f;

        world_update(world, &timer);
        world_render(world, &timer);

        timer.current_time = current_time;
    }

    SDL_Quit();
    return 0;
}
