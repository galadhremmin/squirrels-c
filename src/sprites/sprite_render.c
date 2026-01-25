#include <SDL3/SDL.h>

#include "sprite_animate.h"
#include "sprite_render.h"

void sprite_render(const SDL_Renderer* renderer,
                   const Sprite* sprite,
                   const SpriteAnimationFace face,
                   const uint8_t frame_number,
                   const SDL_FRect* dst_rect) {
    SDL_FRect src_rect = {0, 0, 0, 0};
    sprite_animate_offset_rect(&src_rect, sprite, face, frame_number);
    SDL_RenderTexture((SDL_Renderer*)renderer, sprite->frames_texture, &src_rect, dst_rect);
}
