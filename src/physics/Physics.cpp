#include "Physics.h"

#include "../agent/Agent.h"

void Physics::step(Agent& agent, const Timer& timer) const {
    agent.velocity_.y += gravity_ * timer.delta_time;
    agent.position_.x += agent.velocity_.x * timer.delta_time;
    agent.position_.y += agent.velocity_.y * timer.delta_time;
}

void Physics::applyJumpImpulse(Agent& agent) const {
    agent.velocity_.y = -jump_speed_;
}
