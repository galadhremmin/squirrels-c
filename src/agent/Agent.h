#pragma once

#include <memory>
#include <string>

#include "../sprites/Sprite.h"
#include "../states/StateMachine.h"

class Agent {
  public:
    Agent() = default;
    explicit Agent(std::string n) : name_(std::move(n)) {
    }

    // Direct member access (World owns sprites, agents hold raw pointers)
    std::string name_;
    Sprite* sprite_ = nullptr;
    float position_x_ = 0.0f;
    float position_y_ = 0.0f;
    float velocity_x_ = 0.0f;
    float velocity_y_ = 0.0f;
    float direction_ = 0.0f;
    SpriteAnimationState animation_state_{};
};
