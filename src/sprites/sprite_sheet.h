#pragma once

#include <SDL3/SDL.h>
#include <stdint.h>

#include "../utils/paths.h"

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
    const uint8_t frame_count;
    uint8_t offset_indices_by_animation[SPRITE_ANIMATION_COUNT];
    const size_t frame_width;
    const size_t frame_height;
} SpriteSheet;

SpriteSheet* sprite_sheet_new(const SDL_Renderer* renderer,
                              const char* sprite_sheet_name,
                              const size_t frame_width,
                              const size_t frame_height) __attribute__((nonnull(1, 2)));

void sprite_sheet_animation_add(SpriteSheet* sprite_sheet,
                                const SpriteAnimation animation,
                                const uint8_t offset_index) __attribute__((nonnull(1)));

void sprite_sheet_free(SpriteSheet** sprite_sheet);
