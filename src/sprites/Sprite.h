#pragma once

#include <SDL3/SDL.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>

#include "../utils/Timer.h"

typedef enum {
    SPRITE_ANIMATION_FACE_FRONT,
    SPRITE_ANIMATION_FACE_BACK,
    SPRITE_ANIMATION_FACE_LEFT,
    SPRITE_ANIMATION_FACE_RIGHT
} SpriteAnimationFace;

typedef struct AnimationState {
    SpriteAnimationFace face;
    uint64_t last_frame_time;
    float fps;
    uint8_t frame_number;
} AnimationState;

class Sprite {
  public:
    explicit Sprite(const size_t frame_width, const size_t frame_height);
    ~Sprite();

    // Not copyable, but movable since sprites retains SDL texture pointers which shouldn't be
    // copied since they are unique to the sprite.
    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;
    Sprite(Sprite&&) noexcept;
    Sprite& operator=(Sprite&&) noexcept;

    void load(const std::weak_ptr<SDL_Renderer>& renderer_ptr, const std::string& sprite_filename);
    void add_animation(const SpriteAnimationFace face, const uint8_t offset_index);
    uint8_t get_offset_index(const SpriteAnimationFace face) const;
    void update_animation_state(AnimationState& state, const Timer& timer) const;

    uint8_t frame_count() const {
        return frame_count_;
    }
    size_t frame_width() const {
        return frame_width_;
    }
    size_t frame_height() const {
        return frame_height_;
    }
    bool loaded() const {
        return loaded_;
    }
    SDL_Texture* texture() const {
        return frames_texture_ptr_.get();
    }

  private:
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> frames_texture_ptr_;
    uint8_t frame_count_;
    std::unordered_map<SpriteAnimationFace, uint8_t> offset_indices_by_animation_face_;
    size_t frame_width_;
    size_t frame_height_;
    bool loaded_;
};
