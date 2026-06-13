#pragma once

#include <chrono>

#include "../sprites/Sprite.h"
#include "Agent.h"

class BirdAgent : public Agent {
  public:
    BirdAgent(squirrel::Vector2f position,
              SpriteAnimationState animation_state,
              Sprite* bird_sprite,
              Sprite* egg_sprite)
        : Agent("bird", position, squirrel::Sizef{.w = 0.05f, .h = 0.09f}, animation_state),
          egg_sprite_(egg_sprite) {
        setSprite(bird_sprite);
        setIsFlying(true);
    }

    virtual ~BirdAgent() = default;

    void update(const Timer& timer, AgentManager& manager) override;
    void reset(const ViewportBounds& bounds);
    void onViewportBoundaryCollision(const ViewportBounds& bounds, ViewportEdge edges) override;

  private:
    float elapsed_ = 0.0f;
    std::chrono::nanoseconds last_egg_spawned_at_{};
    Sprite* egg_sprite_;
};
