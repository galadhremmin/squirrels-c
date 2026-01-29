#pragma once

#include <stddef.h>

#define ASSETS_DIR "assets"
#define SPRITES_DIR ASSETS_DIR "/sprites"
#define TEXTURES_DIR ASSETS_DIR "/textures"
#define SOUNDS_DIR ASSETS_DIR "/sounds"
#define FONTS_DIR ASSETS_DIR "/fonts"

size_t path_for_sprite(char* buffer, const size_t buffer_size, const char* sprite_name)
    __attribute__((nonnull(1, 3)));
