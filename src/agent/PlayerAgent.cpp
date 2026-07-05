#include "PlayerAgent.h"
#include "AgentManager.h"
#include <print>

void PlayerAgent::processInput(const SDL_Event& event) {
    const bool keydown = (event.type == SDL_EVENT_KEY_DOWN);
    const bool keyup = (event.type == SDL_EVENT_KEY_UP);

    if ((!keydown && !keyup) || event.key.repeat)
        return;

    switch (event.key.scancode) {
    case SDL_SCANCODE_LEFT:
    case SDL_SCANCODE_A:
        input_left_ = keydown;
        break;
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_D:
        input_right_ = keydown;
        break;
    case SDL_SCANCODE_SPACE:
        input_jump_ = keydown;
        break;
    default:
        break;
    }
}

void PlayerAgent::onViewportBoundaryCollision(const ViewportBounds& bounds, ViewportEdge edges) {
    Agent::onViewportBoundaryCollision(bounds, edges);

    if (edges & ViewportEdge::Left) {
        position_.x = bounds.left;
        if (velocity_.x < 0.0f)
            velocity_.x = 0.0f;
    }
    if (edges & ViewportEdge::Right) {
        position_.x = bounds.right - size_.w;
        if (velocity_.x > 0.0f)
            velocity_.x = 0.0f;
    }
}

void PlayerAgent::onAgentCollision(Agent& agent, AgentManager& manager) {
    if (agent.getName() == "egg") {
        manager.removeDeferred(agent);
        std::println("Caught egg {}!", reinterpret_cast<void*>(&agent));
    }
}

void PlayerAgent::update([[maybe_unused]] const Timer& timer,
                         [[maybe_unused]] AgentManager& manager) {
    if (input_left_) {
        getMutableVelocity().x = -kMoveSpeed;
    } else if (input_right_) {
        getMutableVelocity().x = kMoveSpeed;
    } else {
        getMutableVelocity().x = 0.0f;
    }

    bool started_jumping = false;
    if (input_jump_ && getIsGrounded()) {
        physics_.applyJumpImpulse(*this);
        started_jumping = true;
    }

    AgentStateId anim_state;
    SpriteAnimationFace face;

    if (getVelocity().x < 0.0f) {
        anim_state = AgentStateId::Running;
        face = SpriteAnimationFace::Left;
    } else if (getVelocity().x > 0.0f) {
        anim_state = AgentStateId::Running;
        face = SpriteAnimationFace::Right;
    } else {
        anim_state = AgentStateId::Idle;
        face = SpriteAnimationFace::Front;
    }

    if (started_jumping || anim_state != prev_anim_state_ || face != prev_face_) {
        Sprite* next_sprite =
            (anim_state == AgentStateId::Running)
                ? ((is_grounded_ && !started_jumping) ? run_shadow_sprite_ : run_sprite_)
                : ((is_grounded_ && !started_jumping) ? idle_shadow_sprite_ : idle_sprite_);
        if (getSprite() != next_sprite) {
            setSprite(next_sprite);
        }
        Sprite::resetAnimationState(getMutableAnimationState(), face);
        prev_anim_state_ = anim_state;
        prev_face_ = face;
    }
}
