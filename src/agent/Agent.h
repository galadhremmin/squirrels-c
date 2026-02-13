#pragma once

#include <string>

#include "../physics/Polar.h"
#include "../physics/Vector.h"
#include "../sprites/Sprite.h"
#include "../states/StateMachine.h"

class Agent {
  public:
    Agent() = default;
    explicit Agent(std::string n) : name_(std::move(n)) {
    }

    std::string name_;
    Sprite* sprite_ = nullptr;
    squirrel::Vector2f position_{};
    squirrel::Polar2f velocity_{};
    SpriteAnimationState animation_state_{};
};
