#pragma once

#include "../utils/Timer.h"

class Agent;

class Physics {
  public:
    void setGravity(float g) {
        gravity_ = g;
    }

    float getGravity() const {
        return gravity_;
    }

    void update(Agent& agent, const Timer& timer) const;
    void applyJumpImpulse(Agent& agent) const;

  private:
    float gravity_ = 0.28f;    // normalized units/s²  (≈ 200px/s² at 720p)
    float jump_speed_ = 0.56f; // normalized units/s   (≈ 400px/s at 720p)
};
