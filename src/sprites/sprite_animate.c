#include <SDL3/SDL.h>

#include "../utils/debug.h"
#include "sprite.h"
#include "sprite_animate.h"

void sprite_animate_add(Sprite* const sprite,
                        const SpriteAnimation animation,
                        const uint8_t offset_index) {
    if (animation == 0 || animation >= SPRITE_ANIMATION_COUNT) {
        DEBUG_LOG("Invalid animation %d since it is not in the range of %d to %d",
                  animation,
                  1,
                  SPRITE_ANIMATION_COUNT - 1);
        return;
    }

    sprite->offset_indices_by_animation[animation - 1] = offset_index;
}

void sprite_animate_offset_rect(SDL_FRect* const src_rect,
                                const Sprite* sprite,
                                const SpriteAnimation animation,
                                const uint8_t frame_number) {
    src_rect->x = (float)(frame_number * sprite->frame_width);
    // animation is 1-indexed, so we subtract 1 to get the offset index
    uint8_t offset_index = animation - 1;
    src_rect->y = (float)(sprite->offset_indices_by_animation[offset_index] * sprite->frame_height);
    src_rect->w = (float)sprite->frame_width;
    src_rect->h = (float)sprite->frame_height;
}

void sprite_animate_update(const Sprite* const sprite,
                           AnimationState* const animation_state,
                           const Timer* timer) {
    float frame_duration_ms = 1000 / animation_state->fps;
    float elapsed_time_ms = (timer->current_time - animation_state->last_frame_time) / 1000000.0f;

    if (elapsed_time_ms >= frame_duration_ms) {
        animation_state->frame_number = (animation_state->frame_number + 1) % sprite->frame_count;
        animation_state->last_frame_time = timer->current_time;
    }
}
