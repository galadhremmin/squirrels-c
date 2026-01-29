#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "../utils/Timer.h"
#include "Sprite.h"

static SDL_Texture* sprite_texture_load(SDL_Renderer* renderer, const char* sprite_sheet_name);

Sprite::Sprite(const size_t frame_width, const size_t frame_height)
    : frames_texture_ptr_(nullptr, SDL_DestroyTexture), frame_count_(0), frame_width_(frame_width),
      frame_height_(frame_height), loaded_(false) {
    if (frame_width_ == 0 || frame_height_ == 0) {
        throw std::invalid_argument("Invalid frame width or height");
    }
}

Sprite::~Sprite() {
}

Sprite::Sprite(Sprite&& other) noexcept
    : frames_texture_ptr_(std::move(other.frames_texture_ptr_)), frame_count_(other.frame_count_),
      frame_width_(other.frame_width_), frame_height_(other.frame_height_), loaded_(other.loaded_),
      offset_indices_by_animation_face_(std::move(other.offset_indices_by_animation_face_)) {
    other.frame_count_ = 0;
    other.frame_width_ = 0;
    other.frame_height_ = 0;
    other.loaded_ = false;
}

Sprite& Sprite::operator=(Sprite&& other) noexcept {
    if (this != &other) {
        frames_texture_ptr_ = std::move(other.frames_texture_ptr_);
        frame_count_ = other.frame_count_;
        frame_width_ = other.frame_width_;
        frame_height_ = other.frame_height_;
        loaded_ = other.loaded_;
        offset_indices_by_animation_face_ = std::move(other.offset_indices_by_animation_face_);

        other.frame_count_ = 0;
        other.frame_width_ = 0;
        other.frame_height_ = 0;
        other.loaded_ = false;
    }
    return *this;
}

void Sprite::load(const std::weak_ptr<SDL_Renderer>& renderer_ptr,
                  const std::string& sprite_filename) {
    if (loaded_) {
        return;
    }

    auto renderer = renderer_ptr.lock();
    if (!renderer) {
        throw std::runtime_error("Renderer is not valid");
    }

    auto full_sprite_path = "assets/sprites/" + sprite_filename + ".png";

    SDL_Texture* texture = IMG_LoadTexture(renderer.get(), full_sprite_path.c_str());
    if (texture == nullptr) {
        SDL_Log("Failed to load sprite sheet '%s': %s", full_sprite_path.c_str(), SDL_GetError());
        return;
    }

    SDL_Log("[DEBUG] Successfully loaded sprite: %s\n", full_sprite_path.c_str());

    float texture_w, texture_h;
    SDL_GetTextureSize(texture, &texture_w, &texture_h);

    if (texture_w <= 0.0f || texture_h <= 0.0f) {
        SDL_DestroyTexture(texture);
        SDL_Log("Invalid texture dimensions as they are not positive.");
        return;
    }

    if (static_cast<int>(texture_w) % static_cast<int>(frame_width_) > 0 ||
        static_cast<int>(texture_h) % static_cast<int>(frame_height_) > 0) {
        SDL_DestroyTexture(texture);
        SDL_Log("Invalid texture dimensions as they are not multiples of frame size.");
        return;
    }

    size_t frame_count = static_cast<size_t>(texture_w) / frame_width_;

    if (frame_count == 0 || frame_count > UINT8_MAX) {
        SDL_DestroyTexture(texture);
        SDL_Log("Invalid frame count calculated.");
        return;
    }

    frame_count_ = static_cast<uint8_t>(frame_count);
    frames_texture_ptr_.reset(texture);
    loaded_ = true;
}

void Sprite::add_animation(const SpriteAnimationFace face, const uint8_t offset_index) {
    offset_indices_by_animation_face_.insert({face, offset_index});
}

uint8_t Sprite::get_offset_index(const SpriteAnimationFace face) const {
    if (offset_indices_by_animation_face_.find(face) == offset_indices_by_animation_face_.end()) {
        throw std::runtime_error("Offset index not found for face");
    }
    return offset_indices_by_animation_face_.at(face);
}

void Sprite::update_animation_state(AnimationState& state, const Timer& timer) const {
    float frame_duration_ms = 1000.0f / state.fps;
    float elapsed_time_ms = (timer.current_time_ns - state.last_frame_time) / 1000000.0f;

    if (elapsed_time_ms >= frame_duration_ms) {
        state.frame_number = (state.frame_number + 1) % frame_count_;
        state.last_frame_time = timer.current_time_ns;
    }
}
