#pragma once

#include <SDL3/SDL.h>
#include <stddef.h>

#include "sprite.h"

// Load and create a new sprite from a sprite sheet
Sprite* sprite_new(const SDL_Renderer* renderer,
                   const char* sprite_sheet_name,
                   const size_t frame_width,
                   const size_t frame_height) __attribute__((nonnull(1, 2)));

// Free a sprite and its associated resources
void sprite_free(Sprite** sprite);
