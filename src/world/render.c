#include <SDL3/SDL.h>

#include "../sprites/sprite_render.h"
#include "../utils/debug.h"
#include "world.h"

static void render_agent(const SDL_Renderer* renderer, Agent* const agent, const Timer* timer)
    __attribute__((nonnull(1, 2)));

void world_render_init(World* const world) {
    SDL_Renderer* renderer = (SDL_Renderer*)world->renderer;

    const int enable_vsync = 1;
    if (!SDL_SetRenderVSync(renderer, enable_vsync)) {
        DEBUG_LOG("VSync set to %d failed: %s", enable_vsync, SDL_GetError());
    }
}

void world_render(World* const world, const Timer* timer) {
    SDL_Renderer* renderer = (SDL_Renderer*)world->renderer;
    SDL_SetRenderDrawColor(renderer, 30, 60, 90, 255);
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < world->agent_count; i++) {
        if (world->agents[i] != NULL) {
            render_agent(renderer, world->agents[i], timer);
        }
    }

    SDL_RenderPresent(renderer);
}

static void render_agent(const SDL_Renderer* renderer, Agent* const agent, const Timer* timer) {
    if (agent->sprite != NULL) {
        sprite_animate_update(agent->sprite, &agent->animation_state, timer);

        SDL_FRect dst_rect = {
            .x = agent->position_x,
            .y = agent->position_y,
            .w = (float)agent->sprite->frame_width * 2,
            .h = (float)agent->sprite->frame_height * 2,
        };

        sprite_render(renderer,
                      agent->sprite,
                      agent->animation_state.animation,
                      agent->animation_state.frame_number,
                      &dst_rect);
    }
}