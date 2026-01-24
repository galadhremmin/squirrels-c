#include <assert.h>
#include <SDL3/SDL.h>

#include "sprite_render.h"

void sprite_sheet_animation_offset_rect(SDL_FRect* src_rect,
                                        const SpriteSheet* sprite_sheet,
                                        const SpriteAnimation animation,
                                        const uint8_t frame_number) {
    src_rect->x = (float)(frame_number * sprite_sheet->frame_width);
    // animation is 1-indexed, so we subtract 1 to get the offset index
    uint8_t offset_index = animation - 1;
    src_rect->y = (float)(sprite_sheet->offset_indices_by_animation[offset_index] *
                          sprite_sheet->frame_height);
    src_rect->w = (float)sprite_sheet->frame_width;
    src_rect->h = (float)sprite_sheet->frame_height;
}

void sprite_sheet_animation_next_frame(SpriteSheet* sprite_sheet,
                                       uint8_t* frame_number) {
    *frame_number = (*frame_number + 1) % sprite_sheet->frame_count;
}

void sprite_sheet_render(const SDL_Renderer* renderer,
                         const SpriteSheet* sprite_sheet,
                         const SpriteAnimation animation,
                         const uint8_t frame_number,
                         const SDL_FRect* dst_rect) {
    SDL_FRect src_rect = {0, 0, 0, 0};
    sprite_sheet_animation_offset_rect(&src_rect, sprite_sheet, animation, frame_number);
    SDL_RenderTexture((SDL_Renderer*)renderer, sprite_sheet->frames_texture, &src_rect, dst_rect);
}
