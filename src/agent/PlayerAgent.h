#pragma once

#include <SDL3/SDL.h>

#include "../physics/Physics.h"
#include "../sprites/Sprite.h"
#include "Agent.h"
#include "AgentState.h"

class PlayerAgent : public Agent {
  public:
    PlayerAgent(squirrel::Vector2f position,
                SpriteAnimationState animation_state,
                Sprite* idle_sprite,
                Sprite* idle_shadow_sprite,
                Sprite* run_sprite,
                Sprite* run_shadow_sprite,
                Physics& physics);

    void processInput(const SDL_Event& event);
    void update(const Timer& timer) override;
    void onViewportBoundaryCollision(const ViewportBounds& bounds, ViewportEdge edges) override;

  private:
    static constexpr float kMoveSpeed = 0.12f;  // normalized units/s  (≈ 150px/s at 1280p)

    Physics& physics_;
    Sprite* idle_sprite_;
    Sprite* idle_shadow_sprite_;
    Sprite* run_sprite_;
    Sprite* run_shadow_sprite_;

    bool input_left_ = false;
    bool input_right_ = false;
    bool input_jump_ = false;

    AgentStateId prev_anim_state_ = AgentStateId::Idle;
    SpriteAnimationFace prev_face_ = SpriteAnimationFace::Front;
};
