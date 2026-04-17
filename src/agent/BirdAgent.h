#pragma once

#include "../sprites/Sprite.h"
#include "Agent.h"

class BirdAgent : public Agent {
  public:
    BirdAgent(squirrel::Vector2f position, SpriteAnimationState animation_state, Sprite* sprite);

    void update(const Timer& timer) override;
    void reset(const ViewportBounds& bounds);
    void onViewportBoundaryCollision(const ViewportBounds& bounds, uint8_t edges) override;

  private:
    float elapsed_ = 0.0f;
};
