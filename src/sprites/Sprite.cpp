#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "../utils/Timer.h"
#include "Sprite.h"

Sprite::Sprite(const squirrel::Texture* frames_texture_ptr,
               const size_t frame_width,
               const size_t frame_height)
    : frames_texture_ptr_(frames_texture_ptr), frame_count_(0), frame_width_(frame_width),
      frame_height_(frame_height) {
    if (frame_width_ == 0 || frame_height_ == 0) {
        throw std::invalid_argument("Invalid frame width or height");
    }
    if (frames_texture_ptr_ == nullptr) {
        throw std::invalid_argument("Frames texture pointer must not be null");
    }
    reset(frames_texture_ptr);
}

Sprite::~Sprite() {
}

void Sprite::reset(const squirrel::Texture* frames_texture_ptr) {
    if (frames_texture_ptr_ == nullptr) {
        throw std::invalid_argument("Frames texture pointer must not be null");
    }

    float texture_w, texture_h;
    SDL_GetTextureSize(frames_texture_ptr->texture, &texture_w, &texture_h);

    if (texture_w <= 0.0f || texture_h <= 0.0f) {
        throw std::invalid_argument("Invalid texture dimensions as they are not positive.");
    }

    if (static_cast<int>(texture_w) % static_cast<int>(frame_width_) > 0 ||
        static_cast<int>(texture_h) % static_cast<int>(frame_height_) > 0) {
        throw std::invalid_argument(
            "Invalid texture dimensions as they are not multiples of frame size.");
    }

    size_t frame_count = static_cast<size_t>(texture_w) / frame_width_;

    if (frame_count == 0 || frame_count > UINT8_MAX) {
        throw std::invalid_argument("Invalid frame count calculated.");
    }

    frame_count_ = static_cast<uint8_t>(frame_count);
    frames_texture_ptr_ = frames_texture_ptr;
}

void Sprite::addAnimation(const SpriteAnimationFace face, const uint8_t offset_index) {
    offset_indices_by_animation_face_.insert({face, offset_index});
}

uint8_t Sprite::getOffsetIndex(const SpriteAnimationFace face) const {
    if (offset_indices_by_animation_face_.find(face) == offset_indices_by_animation_face_.end()) {
        throw std::runtime_error("Offset index not found for face");
    }
    return offset_indices_by_animation_face_.at(face);
}

void Sprite::updateAnimationState(SpriteAnimationState& state, const Timer& timer) const {
    float frame_duration_ms = 1000.0f / state.fps;
    float elapsed_time_ms = (timer.current_time_ns - state.last_frame_time) / 1000000.0f;

    if (elapsed_time_ms >= frame_duration_ms) {
        state.frame_number = (state.frame_number + 1) % frame_count_;
        state.last_frame_time = timer.current_time_ns;
    }
}

void Sprite::resetAnimationState(SpriteAnimationState& state, const SpriteAnimationFace face) {
    state.face = face;
    state.frame_number = 0;
    state.last_frame_time = 0;
}
