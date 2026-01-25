#pragma once

#include <SDL3/SDL.h>
#include <stdint.h>

typedef enum {
    SPRITE_ANIMATION_FACE_FRONT = 1,
    SPRITE_ANIMATION_FACE_BACK,
    SPRITE_ANIMATION_FACE_LEFT,
    SPRITE_ANIMATION_FACE_RIGHT,
    SPRITE_ANIMATION_FACE_COUNT
} SpriteAnimationFace;

// Sprite sheet structure
typedef struct {
    SDL_Texture* frames_texture;
    uint8_t frame_count;
    uint8_t offset_indices_by_animation_face[SPRITE_ANIMATION_FACE_COUNT - 1];
    size_t frame_width;
    size_t frame_height;
} Sprite;
