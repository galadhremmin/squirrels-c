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

    void step(Agent& agent, const Timer& timer) const;
    void applyJumpImpulse(Agent& agent) const;

  private:
    float gravity_ = 200.0f;
    float jump_speed_ = 400.0f;
};
