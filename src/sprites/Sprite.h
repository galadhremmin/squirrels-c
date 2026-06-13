#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>

#include "../renderer/Texture.h"
#include "../utils/Timer.h"

enum class SpriteAnimationFace : uint8_t {
    Front,
    Back,
    Left,
    Right,
};

struct SpriteAnimationState {
    SpriteAnimationFace face;
    std::chrono::nanoseconds last_frame_time{};
    float fps = 0;
    uint8_t frame_number = 0;
};

class Sprite {
  public:
    explicit Sprite(const squirrel::Texture* frames_texture_ptr,
                    const size_t frame_width,
                    const size_t frame_height);
    ~Sprite();

    // Not copyable, but movable since sprites retains SDL texture pointers which shouldn't be
    // copied since they are unique to the sprite.
    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;
    Sprite(Sprite&&) noexcept = default;
    Sprite& operator=(Sprite&&) noexcept = default;

    void reset(const squirrel::Texture* frames_texture_ptr);
    void addAnimation(SpriteAnimationFace face, uint8_t offset_index);
    [[nodiscard]] uint8_t getOffsetIndex(SpriteAnimationFace face) const;

    void updateAnimationState(SpriteAnimationState& state, const Timer& timer) const;
    static void resetAnimationState(SpriteAnimationState& state, SpriteAnimationFace face);

    [[nodiscard]] uint8_t getFrameCount() const { return frame_count_; }
    [[nodiscard]] size_t getFrameWidth() const { return frame_width_; }
    [[nodiscard]] size_t getFrameHeight() const { return frame_height_; }
    [[nodiscard]] SDL_Texture* getTexturePtr() const { return frames_texture_ptr_->texture; }

  private:
    const squirrel::Texture* frames_texture_ptr_;
    uint8_t frame_count_;
    size_t frame_width_;
    size_t frame_height_;
    std::array<uint8_t, 4> offset_indices_{};
};
