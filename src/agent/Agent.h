#pragma once

#include <string>

#include "../physics/Vector.h"
#include "../sprites/Sprite.h"

class Agent {
    friend class Physics;

  public:
    Agent() = default;
    explicit Agent(std::string n, squirrel::Vector2f position, SpriteAnimationState animation_state)
        : name_(std::move(n)), position_(std::move(position)),
          animation_state_(std::move(animation_state)) {
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

    void setSprite(Sprite* const sprite) {
        sprite_ = sprite;
    }

    void setIsGrounded(const bool grounded) {
        is_grounded_ = grounded;
    }

  private:
    std::string name_;
    Sprite* sprite_ = nullptr;
    squirrel::Vector2f position_{};
    squirrel::Vector2f velocity_{};
    SpriteAnimationState animation_state_{};
    bool is_grounded_ = false;
};
