#pragma once

#include <string>

#include "../physics/Vector.h"
#include "../sprites/Sprite.h"
#include "../utils/Timer.h"
#include "ViewportBoundary.h"

class Agent {
    friend class Physics;

  public:
    virtual ~Agent() = default;

    virtual void update(const Timer& timer) = 0;
    virtual void onViewportBoundaryCollision(const ViewportBounds& bounds, uint8_t edges) {
    }

    const std::string& getName() const {
        return name_;
    }

    const Sprite* getSprite() const {
        return sprite_;
    }

    const squirrel::Vector2f& getPosition() const {
        return position_;
    }

    squirrel::Vector2f& getMutablePosition() {
        return position_;
    }

    const squirrel::Vector2f& getVelocity() const {
        return velocity_;
    }

    squirrel::Vector2f& getMutableVelocity() {
        return velocity_;
    }

    const SpriteAnimationState& getAnimationState() const {
        return animation_state_;
    }

    SpriteAnimationState& getMutableAnimationState() {
        return animation_state_;
    }

    bool getIsGrounded() const {
        return is_grounded_;
    }

    bool getIsFlying() const {
        return is_flying_;
    }

    void setSprite(Sprite* const sprite) {
        sprite_ = sprite;
    }

    void setIsGrounded(const bool grounded) {
        is_grounded_ = grounded;
    }

    void setIsFlying(bool is_flying) {
        is_flying_ = is_flying;
    }

  protected:
    explicit Agent(std::string n, squirrel::Vector2f position, SpriteAnimationState animation_state)
        : name_(std::move(n)), position_(std::move(position)),
          animation_state_(std::move(animation_state)) {
    }

    std::string name_;
    Sprite* sprite_ = nullptr;
    squirrel::Vector2f position_{};
    squirrel::Vector2f velocity_{};
    SpriteAnimationState animation_state_{};
    bool is_grounded_ = false;
    bool is_flying_ = false;
};
