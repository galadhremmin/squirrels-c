#define _POSIX_C_SOURCE 200809L  // For PATH_MAX
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <assert.h>
#include <limits.h>  // For PATH_MAX
#include <stdlib.h>
#include <string.h>

#include "../utils/debug.h"
#include "../utils/paths.h"
#include "sprite_sheet.h"

static SDL_Texture* sprite_texture_load(const SDL_Renderer* renderer,
                                        const char* sprite_sheet_name);

SpriteSheet* sprite_sheet_new(const SDL_Renderer* renderer,
                              const char* sprite_sheet_name,
                              const size_t frame_width,
                              const size_t frame_height) {
    if (frame_width == 0 || frame_height == 0) {
        DEBUG_LOG("Invalid frame width or height (frame_width: %zu, frame_height: %zu)", frame_width, frame_height);
        return NULL;
    }

    SDL_Texture* texture = sprite_texture_load(renderer, sprite_sheet_name);
    if (texture == NULL) {
        DEBUG_LOG("Failed to load sprite sheet '%s': %s", sprite_sheet_name, SDL_GetError());
        return NULL;
    }

    float texture_w, texture_h;
    SDL_GetTextureSize(texture, &texture_w, &texture_h);

    if (texture_w <= 0.0f || texture_h <= 0.0f) {
        DEBUG_LOG("%s", "Invalid texture dimensions");
        goto error;
    }

    if ((int)texture_w % (int)frame_width > 0 || (int)texture_h % (int)frame_height > 0) {
        DEBUG_LOG("%s", "Texture width or height is not a multiple of frame_width or frame_height");
        goto error;
    }

    size_t frame_count = (size_t)texture_w / frame_width;
    
    if (frame_count == 0 || frame_count > UINT8_MAX) {
        DEBUG_LOG("%s", "Invalid frame count calculated");
        goto error;
    }

    SpriteSheet sheet = {.frames_texture = texture,
                         .frame_count = (uint8_t)frame_count,
                         .offset_indices_by_animation = {0},
                         .frame_width = frame_width,
                         .frame_height = frame_height};

    SpriteSheet* sprite_sheet = (SpriteSheet*)malloc(sizeof(SpriteSheet));
    if (sprite_sheet == NULL) {
        goto error;
    }

    memcpy(sprite_sheet, &sheet, sizeof(SpriteSheet));
    return sprite_sheet;

error:
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }
    if (sprite_sheet != NULL) {
        free(sprite_sheet);
    }
    return NULL;
}

void sprite_sheet_animation_add(SpriteSheet* sprite_sheet,
                                const SpriteAnimation animation,
                                const uint8_t offset_index) {
    if (animation == 0 || animation >= SPRITE_ANIMATION_COUNT) {
        DEBUG_LOG("Invalid animation %d since it is not in the range of %d to %d", animation, 1, SPRITE_ANIMATION_COUNT);
        return;
    }

    sprite_sheet->offset_indices_by_animation[animation - 1] = offset_index;
}

void sprite_sheet_free(SpriteSheet** sprite_sheet) {
    if (sprite_sheet == NULL || *sprite_sheet == NULL) {
        return;
    }

    if ((*sprite_sheet)->frames_texture != NULL) {
        SDL_DestroyTexture((*sprite_sheet)->frames_texture);
    }

    free(*sprite_sheet);
    *sprite_sheet = NULL;
}

static SDL_Texture* sprite_texture_load(const SDL_Renderer* renderer,
                                        const char* sprite_sheet_name) {
    char sprite_sheet_path[PATH_MAX + 1] = {0};
    path_for_sprite(sprite_sheet_path, sizeof(sprite_sheet_path), sprite_sheet_name);

    DEBUG_LOG("Loading sprite: %s", sprite_sheet_path);

    SDL_Texture* texture = IMG_LoadTexture((SDL_Renderer*)renderer, sprite_sheet_path);
    if (texture == NULL) {
        DEBUG_LOG("Failed to load sprite sheet '%s': %s", sprite_sheet_path, SDL_GetError());
        return NULL;
    }

    DEBUG_LOG("[DEBUG] Successfully loaded sprite: %s\n", sprite_sheet_path);
    return texture;
}
