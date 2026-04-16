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
    const float sprite_w = sprite_ ? static_cast<float>(sprite_->getFrameWidth()) : 0.0f;
    const float sprite_h = sprite_ ? static_cast<float>(sprite_->getFrameHeight()) : 0.0f;

    std::bernoulli_distribution coin{0.5};
    if (coin(rng_)) {
        // left
        position_.x = -sprite_w;
        velocity_.x = 120.0f;
        animation_state_.face = SPRITE_ANIMATION_FACE_RIGHT;
    } else {
        // right
        position_.x = bounds.right + sprite_w;
        velocity_.x = -120.0f;
        animation_state_.face = SPRITE_ANIMATION_FACE_LEFT;
    }
    // position_.y = sprite_h + 0;
}