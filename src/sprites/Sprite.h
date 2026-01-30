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

struct SpriteAnimationState {
    SpriteAnimationFace face;
    uint64_t last_frame_time;
    float fps;
    uint8_t frame_number;
};

class Sprite {
  public:
    explicit Sprite(const size_t frame_width, const size_t frame_height);
    ~Sprite();

    // Not copyable, but movable since sprites retains SDL texture pointers which shouldn't be
    // copied since they are unique to the sprite.
    Sprite(const Sprite&) = delete;
    Sprite& operator=(const Sprite&) = delete;
    Sprite(Sprite&&) noexcept = default;
    Sprite& operator=(Sprite&&) noexcept = default;

    /**
     * Initializes the sprite by loading it from the given filename.
     * @param sdl_renderer_ptr The SDL renderer to use to load the sprite.
     * @param sprite_filename The filename of the sprite to load without file extension.
     */
    void load(SDL_Renderer* const sdl_renderer_ptr, const std::string& sprite_filename);

    /**
     * Adds the animation for the given face.
     * @param face The face to add the animation for.
     * @param offset_index The offset index of the animation.
     */
    void addAnimation(const SpriteAnimationFace face, const uint8_t offset_index);

    /**
     * Gets the offset index for the given face.
     * @param face The face to get the offset index for.
     * @return The offset index for the given face.
     */
    uint8_t getOffsetIndex(const SpriteAnimationFace face) const;

    /**
     * Updates the animation state for the given state.
     * @param state The state to update the animation for.
     * @param timer The timer to use to update the animation.
     */
    void updateAnimationState(SpriteAnimationState& state, const Timer& timer) const;

    /**
     * Resets the animation state for the given state.
     * @param state The state to reset the animation for.
     * @param face The face to reset the animation for.
     */
    static void resetAnimationState(SpriteAnimationState& state, const SpriteAnimationFace face);

    uint8_t getFrameCount() const {
        return frame_count_;
    }
    size_t getFrameWidth() const {
        return frame_width_;
    }
    size_t getFrameHeight() const {
        return frame_height_;
    }
    bool getIsLoaded() const {
        return loaded_;
    }
    SDL_Texture* getTexturePtr() const {
        return frames_texture_ptr_.get();
    }

  private:
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> frames_texture_ptr_;
    uint8_t frame_count_;
    size_t frame_width_;
    size_t frame_height_;
    bool loaded_;
    std::unordered_map<SpriteAnimationFace, uint8_t> offset_indices_by_animation_face_;
};
