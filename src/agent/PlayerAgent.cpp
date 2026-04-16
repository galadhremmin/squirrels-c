#include "PlayerAgent.h"

PlayerAgent::PlayerAgent(squirrel::Vector2f position,
                         SpriteAnimationState animation_state,
                         Sprite* idle_sprite,
                         Sprite* run_sprite,
                         Physics& physics)
    : Agent("player", position, squirrel::Sizef{.w = 64.0f, .h = 64.0f}, animation_state),
      physics_(physics), idle_sprite_(idle_sprite), run_sprite_(run_sprite) {
    setSprite(idle_sprite_);
}

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

void PlayerAgent::onViewportBoundaryCollision(const ViewportBounds& bounds, uint8_t edges) {
    if (edges & VIEWPORT_EDGE_LEFT) {
        position_.x = bounds.left;
        if (velocity_.x < 0.0f)
            velocity_.x = 0.0f;
    }
    if (edges & VIEWPORT_EDGE_RIGHT) {
        position_.x = bounds.right - size_.w;
        if (velocity_.x > 0.0f)
            velocity_.x = 0.0f;
    }
    if (edges & VIEWPORT_EDGE_BOTTOM) {
        position_.y = bounds.bottom;
        velocity_.y = 0.0f;
        setIsGrounded(true);
    }
}

void PlayerAgent::update(const Timer& timer) {
    (void)timer;

    if (input_left_) {
        getMutableVelocity().x = -kMoveSpeed;
    } else if (input_right_) {
        getMutableVelocity().x = kMoveSpeed;
    } else {
        getMutableVelocity().x = 0.0f;
    }

    if (input_jump_ && getIsGrounded()) {
        physics_.applyJumpImpulse(*this);
    }

    AgentStateId anim_state;
    SpriteAnimationFace face;

    if (getVelocity().x < 0.0f) {
        anim_state = AGENT_STATE_RUNNING;
        face = SPRITE_ANIMATION_FACE_LEFT;
    } else if (getVelocity().x > 0.0f) {
        anim_state = AGENT_STATE_RUNNING;
        face = SPRITE_ANIMATION_FACE_RIGHT;
    } else {
        anim_state = AGENT_STATE_IDLE;
        face = SPRITE_ANIMATION_FACE_FRONT;
    }

    if (anim_state != prev_anim_state_ || face != prev_face_) {
        Sprite* next_sprite = (anim_state == AGENT_STATE_RUNNING) ? run_sprite_ : idle_sprite_;
        if (getSprite() != next_sprite) {
            setSprite(next_sprite);
        }
        Sprite::resetAnimationState(getMutableAnimationState(), face);
        prev_anim_state_ = anim_state;
        prev_face_ = face;
    }
}
