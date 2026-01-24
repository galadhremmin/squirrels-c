#pragma once

#include <SDL3/SDL.h>
#include <stdint.h>

#include "sprite_sheet.h"

// Calculate source rectangle for a specific animation frame
void sprite_sheet_animation_offset_rect(SDL_FRect* src_rect,
                                        const SpriteSheet* sprite_sheet,
                                        const SpriteAnimation animation,
                                        const uint8_t frame_number) __attribute__((nonnull(1, 2)));

// Advance to next frame in animation
void sprite_sheet_animation_next_frame(SpriteSheet* sprite_sheet, uint8_t* frame_number)
    __attribute__((nonnull(1, 2)));

// Render a sprite frame
void sprite_sheet_render(const SDL_Renderer* renderer,
                         const SpriteSheet* sprite_sheet,
                         const SpriteAnimation animation,
                         const uint8_t frame_number,
                         const SDL_FRect* dst_rect) __attribute__((nonnull(1, 2, 5)));
