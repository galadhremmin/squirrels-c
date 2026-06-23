#pragma once

#include <SDL3/SDL.h>

#include "../sprites/Sprite.h"
#include "../utils/Random.h"
#include "../utils/Timer.h"
#include "Agent.h"

class EggAgent : public Agent {
  public:
    EggAgent(squirrel::Vector2f position, Sprite* sprite, const Timer& timer)
        : Agent("egg", position, {0.025f, 0.025f}, {}),
          ttl_{timer.current_time + std::chrono::seconds(static_cast<long>(
                                        squirrel::Random::instance().range(4.0f, 8.0f)))} {
        setSprite(sprite);
    }

    virtual ~EggAgent() = default;

    void update(const Timer& timer, AgentManager& manager) override;

  private:
    const std::chrono::nanoseconds ttl_;
};
