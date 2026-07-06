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
                Physics& physics)
        : Agent("player", position, squirrel::Sizef{.w = 0.05f, .h = 0.09f}, animation_state),
          physics_(physics), idle_sprite_(idle_sprite), idle_shadow_sprite_(idle_shadow_sprite),
          run_sprite_(run_sprite), run_shadow_sprite_(run_shadow_sprite) {
        setSprite(idle_shadow_sprite);
    };

    virtual ~PlayerAgent() = default;

    void processInput(const SDL_Event& event);
    void update(const Timer& timer, AgentManager& manager) override;
    void onViewportBoundaryCollision(const ViewportBounds& bounds, ViewportEdge edges) override;
    void onAgentCollision(Agent& agent, AgentManager& manager) override;

  private:
    static constexpr float kMoveSpeed = 0.2f; // normalized units/s  (≈ 150px/s at 1280p)

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
