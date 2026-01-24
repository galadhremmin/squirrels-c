#include "utils/memcleanup.h"
#include "sprites/sprite_sheet.h"
#include "sprites/sprite_render.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>

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

    SDL_Renderer* renderer AUTO_CLEANUP_FUNC(cleanup_renderer) = SDL_CreateRenderer(window, NULL);

    if (renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SpriteSheet* fox_idle_sprite_sheet AUTO_CLEANUP_FUNC(sprite_sheet_free) =
        sprite_sheet_new(renderer, "fox_idle", 32, 32);
    sprite_sheet_animation_add(fox_idle_sprite_sheet, SPRITE_ANIMATION_IDLE_FRONT, 0);
    sprite_sheet_animation_add(fox_idle_sprite_sheet, SPRITE_ANIMATION_IDLE_BACK, 1);
    sprite_sheet_animation_add(fox_idle_sprite_sheet, SPRITE_ANIMATION_IDLE_LEFT, 2);
    sprite_sheet_animation_add(fox_idle_sprite_sheet, SPRITE_ANIMATION_IDLE_RIGHT, 3);

    bool quit = false;
    SDL_Event e;
    uint8_t frame_number = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    switch (e.key.scancode) {
                        case SDL_SCANCODE_ESCAPE:
                            quit = true;
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }

        // Clear screen with a color (dark blue)
        SDL_SetRenderDrawColor(renderer, 30, 60, 90, 255);
        SDL_RenderClear(renderer);

        // Draw a simple rectangle (white)
        SDL_FRect dst_rect = {300, 100, 32, 32};
        sprite_sheet_animation_next_frame(fox_idle_sprite_sheet, &frame_number);
        sprite_sheet_render(renderer, fox_idle_sprite_sheet, SPRITE_ANIMATION_IDLE_FRONT, frame_number, &dst_rect);

        // Update screen
        SDL_RenderPresent(renderer);

        // Small delay to prevent excessive CPU usage
        SDL_Delay(250); // ~60 FPS
    }

    SDL_Quit();
    return 0;
}
