#pragma once

#include <SDL3/SDL.h>
#include <stdint.h>

typedef enum {
    SPRITE_ANIMATION_IDLE_FRONT = 1,
    SPRITE_ANIMATION_IDLE_BACK,
    SPRITE_ANIMATION_IDLE_LEFT,
    SPRITE_ANIMATION_IDLE_RIGHT,
    SPRITE_ANIMATION_RUN_FRONT,
    SPRITE_ANIMATION_RUN_BACK,
    SPRITE_ANIMATION_RUN_LEFT,
    SPRITE_ANIMATION_RUN_RIGHT,
    SPRITE_ANIMATION_COUNT
} SpriteAnimation;

// Sprite sheet structure
typedef struct {
    SDL_Texture* frames_texture;
    uint8_t frame_count;
    uint8_t offset_indices_by_animation[SPRITE_ANIMATION_COUNT];
    size_t frame_width;
    size_t frame_height;
} Sprite;
