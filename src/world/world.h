#pragma once
#include <SDL3/SDL.h>

#include "../agent/agent.h"
#include "../sprites/sprite.h"
#include "../utils/timer.h"

typedef enum {
    WORLD_SPRITE_TYPE_FOX_IDLE,
    WORLD_SPRITE_TYPE_FOX_RUN,
    WORLD_SPRITE_TYPE_COUNT,
} WorldSpriteType;

typedef struct {
    Sprite* sprites[WORLD_SPRITE_TYPE_COUNT];
    Agent** agents;
    size_t agent_count;
    size_t agent_capacity;
    size_t player_agent_index;

    const SDL_Renderer* renderer;
} World;

World* world_new(const SDL_Renderer* renderer);
void world_free(World** world);
void world_update(World* const world, const Timer* timer);
