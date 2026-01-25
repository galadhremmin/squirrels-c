#pragma once

#include <SDL3/SDL.h>

#include "sprite.h"

// Render a sprite frame
void sprite_render(const SDL_Renderer* renderer,
                   const Sprite* sprite,
                   const SpriteAnimationFace face,
                   const uint8_t frame_number,
                   const SDL_FRect* dst_rect) __attribute__((nonnull(1, 2, 5)));
