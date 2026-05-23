#include "BirdAgent.h"
#include <cmath>
#include <numbers>

#include "../utils/Random.h"

BirdAgent::BirdAgent(squirrel::Vector2f position,
                     SpriteAnimationState animation_state,
                     Sprite* sprite)
    : Agent("bird", position, squirrel::Sizef{.w = 0.05f, .h = 0.09f}, animation_state) {
    setSprite(sprite);
    setIsFlying(true);
}

static constexpr float kWaveFrequency = 1.5f;
static constexpr float kWaveAmplitude = 0.06f;  // normalized  (≈ 40px at 720p)
static constexpr float kVelocityMax = 0.23f;     // normalized  (≈ 300px/s at 1280p)
static constexpr float kVelocityMin = 0.09f;     // normalized  (≈ 120px/s at 1280p)
static constexpr float kYRangeMin = 0.1f;
static constexpr float kYRangeMax = 0.7f;

void BirdAgent::onViewportBoundaryCollision(const ViewportBounds& bounds, uint8_t edges) {
    if (edges & VIEWPORT_OUTSIDE) {
        reset(bounds);
    }
}

void BirdAgent::update(const Timer& timer) {
    elapsed_ += timer.delta_time;
    velocity_.y =
        std::sinf(elapsed_ * kWaveFrequency * 2.0f * std::numbers::pi_v<float>) * kWaveAmplitude;
}

void BirdAgent::reset(const ViewportBounds& bounds) {
    auto& rng = squirrel::Random::instance();

    const float y_min = bounds.bottom * kYRangeMin;
    const float y_max = bounds.bottom * kYRangeMax;

    if (rng.coinFlip()) {
        // left
        position_.x = -size_.w;
        velocity_.x = rng.range(kVelocityMin, kVelocityMax);
        animation_state_.face = SPRITE_ANIMATION_FACE_RIGHT;
    } else {
        // right
        position_.x = bounds.right;
        velocity_.x = -rng.range(kVelocityMin, kVelocityMax);
        animation_state_.face = SPRITE_ANIMATION_FACE_LEFT;
    }

    position_.y = rng.range(y_min, y_max);
    SDL_Log("[Bird] Reset position x=%f y=%f, face=%d, v=%f\n",
            position_.x,
            position_.y,
            animation_state_.face,
            velocity_.x);
}
