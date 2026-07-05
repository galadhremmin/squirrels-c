#include "Agent.h"

void Agent::onViewportBoundaryCollision(const ViewportBounds& bounds, ViewportEdge edges) {
    if (!is_flying_ && (edges & ViewportEdge::Bottom)) {
        position_.y = bounds.bottom - size_.h;
        velocity_.y = 0.0f;
        is_grounded_ = true;
    }
}

void Agent::onAgentCollision([[maybe_unused]] Agent& agent,
                             [[maybe_unused]] AgentManager& manager) {
}