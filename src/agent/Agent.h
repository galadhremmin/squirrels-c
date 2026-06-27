#pragma once

#include <string>

#include "../physics/Size.h"
#include "../physics/Vector.h"
#include "../sprites/Sprite.h"
#include "../utils/Timer.h"
#include "ViewportBoundary.h"

class AgentManager;

class Agent {
    friend class Physics;

  public:
    virtual ~Agent() = default;

    virtual void update(const Timer& timer, AgentManager& manager) = 0;
    virtual void onViewportBoundaryCollision(const ViewportBounds& bounds, ViewportEdge edges);
    virtual void onAgentCollision(const Agent& agent);

    [[nodiscard]] const std::string& getName() const {
        return name_;
    }
    [[nodiscard]] const Sprite* getSprite() const {
        return sprite_;
    }
    [[nodiscard]] const squirrel::Sizef& getSize() const {
        return size_;
    }
    [[nodiscard]] const squirrel::Vector2f& getPosition() const {
        return position_;
    }
    [[nodiscard]] squirrel::Vector2f& getMutablePosition() {
        return position_;
    }
    [[nodiscard]] const squirrel::Vector2f& getVelocity() const {
        return velocity_;
    }
    [[nodiscard]] squirrel::Vector2f& getMutableVelocity() {
        return velocity_;
    }
    [[nodiscard]] const SpriteAnimationState& getAnimationState() const {
        return animation_state_;
    }
    [[nodiscard]] SpriteAnimationState& getMutableAnimationState() {
        return animation_state_;
    }
    [[nodiscard]] bool getIsGrounded() const {
        return is_grounded_;
    }
    [[nodiscard]] bool getIsFlying() const {
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
    explicit Agent(std::string n,
                   squirrel::Vector2f position,
                   squirrel::Sizef size,
                   SpriteAnimationState animation_state)
        : name_(std::move(n)), position_(position), size_(size), animation_state_(animation_state) {
    }

    std::string name_;
    squirrel::Vector2f position_;
    squirrel::Sizef size_;
    SpriteAnimationState animation_state_;
    Sprite* sprite_ = nullptr;
    squirrel::Vector2f velocity_{};
    bool is_grounded_ = false;
    bool is_flying_ = false;
};
