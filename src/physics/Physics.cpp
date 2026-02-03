#include "Physics.h"

#include "../agent/Agent.h"

void Physics::step(Agent& agent, const Timer& timer) const {

    auto vector = calculateVector2f(agent.velocity_);

    applyGravity(agent, vector, timer);

    agent.position_.x += vector.x * timer.delta_time;
    agent.position_.y += vector.y * timer.delta_time;
}

void Physics::applyForce(Agent& agent, const squirrel::Polar2f& force) const {
    auto vector = calculateVector2f(agent.velocity_);

    vector.x += force.magnitude * std::cos(force.direction_radians);
    vector.y += -force.magnitude * std::sin(force.direction_radians);

    agent.velocity_.magnitude = std::sqrt(vector.x * vector.x + vector.y * vector.y);
    agent.velocity_.direction_radians = std::atan2(-vector.y, vector.x);
}

inline void
Physics::applyGravity(Agent& agent, squirrel::Vector2f& vector, const Timer& timer) const {
    vector.y += gravity_ * timer.delta_time;

    agent.velocity_.magnitude = std::sqrt(vector.x * vector.x + vector.y * vector.y);
    agent.velocity_.direction_radians = std::atan2(-vector.y, vector.x);
}

inline squirrel::Vector2f Physics::calculateVector2f(const squirrel::Polar2f& polar) const {
    return {polar.magnitude * std::cos(polar.direction_radians),
            -polar.magnitude * std::sin(polar.direction_radians)};
}
