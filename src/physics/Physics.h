#pragma once

#include <utility>

#include "../utils/Timer.h"
#include "Polar.h"
#include "Vector.h"

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
    void applyForce(Agent& agent, const squirrel::Polar2f& force) const;

  private:
    inline squirrel::Vector2f calculateVector2f(const squirrel::Polar2f& polar) const;
    inline void applyGravity(Agent& agent, squirrel::Vector2f& vector, const Timer& timer) const;

    float gravity_ = 200.0f;
};
