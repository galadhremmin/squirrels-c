#pragma once

#include <SDL3/SDL.h>
#include <stdint.h>

#include "../utils/timer.h"
#include "sprite.h"

typedef struct {
    SpriteAnimationFace face;
    uint64_t last_frame_time;
    float fps;
    uint8_t frame_number;
} AnimationState;

// Add an animation offset to a sprite
void sprite_animate_add(Sprite* const sprite,
                        const SpriteAnimationFace face,
                        const uint8_t offset_index) __attribute__((nonnull(1)));

// Calculate source rectangle for a specific animation frame
void sprite_animate_offset_rect(SDL_FRect* const src_rect,
                                const Sprite* sprite,
                                const SpriteAnimationFace face,
                                const uint8_t frame_number) __attribute__((nonnull(1, 2)));

// Advance to next frame in animation
void sprite_animate_update(const Sprite* sprite,
                           AnimationState* const animation_state,
                           const Timer* timer) __attribute__((nonnull(1, 2)));
