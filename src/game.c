#include <SDL3/SDL.h>

#include "game.h"
#include "utils/memcleanup.h"
#include "utils/timer.h"
#include "world/render.h"
#include "world/world.h"

int game_run(SDL_Window* window, SDL_Renderer* renderer) {
    (void)window;

    World* world AUTO_CLEANUP_FUNC(world_free) = world_new(renderer);
    if (world == NULL) {
        return 1;
    }

    world_render_init(world);

    Timer timer = {
        .current_time_ns = SDL_GetTicksNS(),
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

        uint64_t current_time_ns = SDL_GetTicksNS();
        uint64_t time_diff = current_time_ns - timer.current_time_ns;
        timer.delta_time = (float)time_diff / 1000000000.0f;

        world_update(world, &timer);
        world_render(world, &timer);

        timer.current_time_ns = current_time_ns;
    }

    return 0;
}
