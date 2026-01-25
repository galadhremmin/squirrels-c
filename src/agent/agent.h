#pragma once

#include "../sprites/sprite.h"
#include "../sprites/sprite_animate.h"

typedef struct {
    const char* name;

    Sprite* sprite;

    float position_x;
    float position_y;

    AnimationState animation_state;

} Agent;

Agent* agent_new(const char* name);
void agent_free(Agent** agent);
