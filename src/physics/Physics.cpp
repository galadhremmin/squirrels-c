#include "Physics.h"

#include "../agent/Agent.h"

void Physics::update(Agent& agent, const Timer& timer) const {
    if (!agent.is_flying_) {
        agent.velocity_.y += gravity_ * timer.delta_time;
    }
    agent.position_ += agent.velocity_ * timer.delta_time;
}

void Physics::applyJumpImpulse(Agent& agent) const {
    agent.velocity_.y = -jump_speed_;
}
