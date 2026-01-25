#define _POSIX_C_SOURCE 200809L // For PATH_MAX

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <limits.h>
#include <stdlib.h>

#include "../utils/debug.h"
#include "../utils/paths.h"
#include "sprite.h"
#include "sprite_load.h"

static SDL_Texture* sprite_texture_load(const SDL_Renderer* renderer,
                                        const char* sprite_sheet_name);

Sprite* sprite_new(const SDL_Renderer* renderer,
                   const char* sprite_sheet_name,
                   const size_t frame_width,
                   const size_t frame_height) {
    if (frame_width == 0 || frame_height == 0) {
        DEBUG_LOG("Invalid frame width or height (frame_width: %zu, frame_height: %zu)",
                  frame_width,
                  frame_height);
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

    Sprite* sprite = (Sprite*)calloc(1, sizeof(Sprite));
    if (sprite == NULL) {
        goto error;
    }

    sprite->frames_texture = texture;
    sprite->frame_count = (uint8_t)frame_count;
    sprite->frame_width = frame_width;
    sprite->frame_height = frame_height;

    return sprite;

error:
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }
    if (sprite != NULL) {
        free(sprite);
    }
    return NULL;
}

void sprite_free(Sprite** sprite) {
    if (sprite == NULL || *sprite == NULL) {
        return;
    }

    if ((*sprite)->frames_texture != NULL) {
        SDL_DestroyTexture((*sprite)->frames_texture);
    }

    free(*sprite);
    *sprite = NULL;
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
