#pragma once

#include "../sprites/sprite.h"
#include "../sprites/sprite_animate.h"

typedef struct {
    const char* name;

    Sprite* sprite;

    float position_x;
    float position_y;

    float velocity_x;
    float velocity_y;
    float direction;

    AnimationState animation_state;
    size_t state_machine_id;
} Agent;

Agent* agent_new(const char* name);
void agent_free(Agent** agent);
