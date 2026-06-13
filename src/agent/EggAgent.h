#pragma once

#include <SDL3/SDL.h>
#include <chrono>

#include "../sprites/Sprite.h"
#include "../utils/Timer.h"
#include "Agent.h"

class EggAgent : public Agent {
  public:
    EggAgent(squirrel::Vector2f position, Sprite* sprite, const Timer& timer)
        : Agent("egg", position, {0.025f, 0.025f}, {}), created_at_{timer.current_time} {
        setSprite(sprite);
    }

    virtual ~EggAgent() = default;

    void update(const Timer& timer, AgentManager& manager) override;

    [[nodiscard]] constexpr bool isExpired(const Timer& timer) const {
        return std::chrono::duration<float>{timer.current_time - created_at_}.count() > 5.f;
    }

  private:
    Sprite* egg_sprite_;
    const std::chrono::nanoseconds created_at_;
};
