#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdlib.h>

#include "../agent/agent.h"
#include "../sprites/sprite_animate.h"
#include "../sprites/sprite_load.h"
#include "world.h"

static void world_sprites_init(World* const world);
static void world_agents_init(World* const world);

World* world_new(const SDL_Renderer* renderer) {
    World* world = (World*)calloc(1, sizeof(World));
    if (world == NULL) {
        return NULL;
    }

    world->renderer = renderer;

    world_sprites_init(world);
    world_agents_init(world);

    return world;
}

void world_free(World** world) {
    if (*world == NULL) {
        return;
    }

    for (size_t i = 0; i < WORLD_SPRITE_TYPE_COUNT; i++) {
        sprite_free(&(*world)->sprites[i]);
    }

    if ((*world)->agents != NULL) {
        for (size_t i = 0; i < (*world)->agent_count; i++) {
            if ((*world)->agents[i] != NULL) {
                agent_free(&(*world)->agents[i]);
                free((*world)->agents[i]);
            }
        }

        free((*world)->agents);
    }

    free(*world);
    *world = NULL;
}

void world_update(World* const world, const Timer* timer) {
    // TODO: Implement world update
    (void)world;
    (void)timer;
}

static void world_sprites_init(World* const world) {
    const SDL_Renderer* renderer = world->renderer;

    Sprite* fox_idle_sprite = sprite_new(renderer, "fox_idle", 32, 32);
    sprite_animate_add(fox_idle_sprite, SPRITE_ANIMATION_IDLE_FRONT, 0);
    sprite_animate_add(fox_idle_sprite, SPRITE_ANIMATION_IDLE_BACK, 1);
    sprite_animate_add(fox_idle_sprite, SPRITE_ANIMATION_IDLE_LEFT, 2);
    sprite_animate_add(fox_idle_sprite, SPRITE_ANIMATION_IDLE_RIGHT, 3);

    Sprite* fox_run_sprite = sprite_new(renderer, "fox_run", 32, 32);
    sprite_animate_add(fox_run_sprite, SPRITE_ANIMATION_RUN_FRONT, 0);
    sprite_animate_add(fox_run_sprite, SPRITE_ANIMATION_RUN_BACK, 1);
    sprite_animate_add(fox_run_sprite, SPRITE_ANIMATION_RUN_LEFT, 2);
    sprite_animate_add(fox_run_sprite, SPRITE_ANIMATION_RUN_RIGHT, 3);

    world->sprites[WORLD_SPRITE_TYPE_FOX_IDLE] = fox_idle_sprite;
    world->sprites[WORLD_SPRITE_TYPE_FOX_RUN] = fox_run_sprite;
}

static void world_agents_init(World* const world) {
    const size_t initial_capacity = 10;

    world->agents = (Agent**)calloc(initial_capacity, sizeof(Agent*));
    if (world->agents == NULL) {
        return;
    }

    world->agent_capacity = initial_capacity;
    world->agent_count = 0;
    world->player_agent_index = SIZE_MAX; // Invalid index

    // Create player agent
    Agent* player = agent_new("player");
    if (player == NULL) {
        return;
    }

    player->sprite = world->sprites[WORLD_SPRITE_TYPE_FOX_IDLE];
    player->animation_state = (AnimationState){
        .animation = SPRITE_ANIMATION_IDLE_LEFT,
        .fps = 4,
        .frame_number = 0,
        .last_frame_time = 0,
    };
    player->position_x = 100;
    player->position_y = 100;

    world->agents[0] = player;
    world->agent_count = 1;
    world->player_agent_index = 0;
}
