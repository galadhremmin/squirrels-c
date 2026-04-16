#include "BirdAgent.h"
#include <cmath>
#include <numbers>
#include <random>

BirdAgent::BirdAgent(squirrel::Vector2f position,
                     SpriteAnimationState animation_state,
                     Sprite* sprite)
    : Agent("bird", position, squirrel::Sizef{.w = 64.0f, .h = 64.0f}, animation_state) {
    setSprite(sprite);
    setIsFlying(true);
}

static constexpr float kWaveFrequency = 1.5f;
static constexpr float kWaveAmplitude = 40.0f;
static constexpr float kVelocityMax = 300.0f;
static constexpr float kVelocityMin = 120.0f;
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
    std::uniform_real_distribution<float> velocity_range{
        kVelocityMin,
        kVelocityMax,
    };

    std::uniform_real_distribution<float> y_range{
        std::fmax(bounds.top, bounds.bottom) * kYRangeMin - 1.0f,
        std::fmax(bounds.top, bounds.bottom) * kYRangeMax + 1.0f,
    };

    std::bernoulli_distribution coin{0.5};
    if (coin(rng_)) {
        // left
        position_.x = -size_.w;
        velocity_.x = velocity_range(rng_);
        animation_state_.face = SPRITE_ANIMATION_FACE_RIGHT;
    } else {
        // right
        position_.x = bounds.right;
        velocity_.x = -velocity_range(rng_);
        animation_state_.face = SPRITE_ANIMATION_FACE_LEFT;
    }

    position_.y = y_range(rng_);
    SDL_Log("[Bird] Reset position x=%f y=%f, face=%d, v=%f\n",
            position_.x,
            position_.y,
            animation_state_.face,
            velocity_.x);
}
