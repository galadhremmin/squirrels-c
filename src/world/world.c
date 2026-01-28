#include <SDL3/SDL.h>
#include <SDL3/SDL_scancode.h>
#include <stdint.h>
#include <stdlib.h>

#include "../agent/agent.h"
#include "../agent/agent_state.h"
#include "../sprites/sprite_animate.h"
#include "../sprites/sprite_load.h"
#include "world.h"

static bool world_sprites_init(World* const world);
static bool world_agents_init(World* const world);

World* world_new(const SDL_Renderer* renderer) {
    World* world = (World*)calloc(1, sizeof(World));
    if (world == NULL) {
        return NULL;
    }

    world->renderer = renderer;

    if (!world_sprites_init(world)) {
        return NULL;
    }

    if (!world_agents_init(world)) {
        return NULL;
    }

    return world;
}

void world_free(World** world) {
    if (*world == NULL) {
        return;
    }

    agent_state_free_all();

    if ((*world)->agents != NULL) {
        for (size_t i = 0; i < (*world)->agent_count; i++) {
            Agent** agent = &(*world)->agents[i];
            if (*agent != NULL) {
                agent_free(agent);
            }
        }

        free((*world)->agents);
        (*world)->agents = NULL;
    }

    for (size_t i = 0; i < WORLD_SPRITE_TYPE_COUNT; i++) {
        Sprite** sprite = &(*world)->sprites[i];
        sprite_free(sprite);
    }

    free(*world);
    *world = NULL;
}

void world_update(World* const world, const Timer* timer) {
    for (size_t i = 0; i < world->agent_count; i++) {
        if (world->agents[i] != NULL) {
            Agent* const agent = world->agents[i];
            int new_state_id = agent_state_update(agent, timer);
            if (new_state_id != -1) {
                switch ((AgentStateId)new_state_id) {
                case AGENT_STATE_IDLE:
                    agent->sprite = world->sprites[WORLD_SPRITE_TYPE_FOX_IDLE];
                    break;
                case AGENT_STATE_RUN_LEFT:
                case AGENT_STATE_RUN_RIGHT:
                    agent->sprite = world->sprites[WORLD_SPRITE_TYPE_FOX_RUN];
                    break;
                default:
                    break;
                }
            }

            agent->position_x += agent->velocity_x * timer->delta_time;
            agent->position_y += agent->velocity_y * timer->delta_time;
        }
    }
}

void world_process_input(World* const world, const SDL_Event* event) {
    Agent* const player_agent = world->agents[world->player_agent_index];
    switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
        if (event->key.repeat) {
            break;
        }
        switch (event->key.scancode) {
        case SDL_SCANCODE_LEFT:
            agent_state_push(player_agent, AGENT_STATE_RUN_LEFT);
            break;
        case SDL_SCANCODE_RIGHT:
            agent_state_push(player_agent, AGENT_STATE_RUN_RIGHT);
            break;
        case SDL_SCANCODE_SPACE:
            agent_state_push(player_agent, AGENT_STATE_JUMP);
            break;
        case SDL_SCANCODE_DOWN:
            agent_state_push(player_agent, AGENT_STATE_DYING); // temporary, just for testing
            break;
        default:
            // noop, do nothing
            break;
        }
        break;
    case SDL_EVENT_KEY_UP:
        if (event->key.repeat) {
            break;
        }
        switch (event->key.scancode) {
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_SPACE:
            agent_state_push(player_agent, AGENT_STATE_IDLE);
            break;
        default:
            // noop, do nothing
            break;
        }
    }
}

static bool world_sprites_init(World* const world) {
    const SDL_Renderer* renderer = world->renderer;

    Sprite* fox_idle_sprite = sprite_new(renderer, "fox_idle", 32, 32);
    sprite_animate_add(fox_idle_sprite, SPRITE_ANIMATION_FACE_FRONT, 0);
    sprite_animate_add(fox_idle_sprite, SPRITE_ANIMATION_FACE_BACK, 1);
    sprite_animate_add(fox_idle_sprite, SPRITE_ANIMATION_FACE_LEFT, 2);
    sprite_animate_add(fox_idle_sprite, SPRITE_ANIMATION_FACE_RIGHT, 3);

    Sprite* fox_run_sprite = sprite_new(renderer, "fox_run", 32, 32);
    sprite_animate_add(fox_run_sprite, SPRITE_ANIMATION_FACE_FRONT, 0);
    sprite_animate_add(fox_run_sprite, SPRITE_ANIMATION_FACE_BACK, 1);
    sprite_animate_add(fox_run_sprite, SPRITE_ANIMATION_FACE_RIGHT, 2);
    sprite_animate_add(fox_run_sprite, SPRITE_ANIMATION_FACE_LEFT, 3);

    world->sprites[WORLD_SPRITE_TYPE_FOX_IDLE] = fox_idle_sprite;
    world->sprites[WORLD_SPRITE_TYPE_FOX_RUN] = fox_run_sprite;

    return true;
}

static bool world_agents_init(World* const world) {
    const size_t initial_capacity = 10;

    world->agents = (Agent**)calloc(initial_capacity, sizeof(Agent*));
    if (world->agents == NULL) {
        return false;
    }

    world->agent_capacity = initial_capacity;
    world->agent_count = 0;
    world->player_agent_index = SIZE_MAX; // Invalid index

    // Create player agent
    Agent* player = agent_new("player");
    if (player == NULL) {
        return false;
    }

    player->sprite = world->sprites[WORLD_SPRITE_TYPE_FOX_IDLE];
    player->animation_state = (AnimationState){
        .face = SPRITE_ANIMATION_FACE_RIGHT,
        .fps = 4,
        .frame_number = 0,
        .last_frame_time = 0,
    };
    player->position_x = 100;
    player->position_y = 100;

    if (!agent_state_new(player)) {
        SDL_Log("Failed to create agent state machine for player");
        return false;
    }

    world->agents[0] = player;
    world->agent_count = 1;
    world->player_agent_index = 0;

    return true;
}
