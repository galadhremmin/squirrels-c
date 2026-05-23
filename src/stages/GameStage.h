#pragma once
#include <SDL3/SDL.h>

#include <memory>
#include <unordered_map>

#include "../agent/Agent.h"
#include "../agent/AgentManager.h"
#include "../physics/Physics.h"
#include "../renderer/Background.h"
#include "../renderer/Renderer.h"
#include "../sprites/Sprite.h"
#include "../utils/Timer.h"

class PlayerAgent;

typedef enum {
    WORLD_SPRITE_TYPE_FOX_IDLE,
    WORLD_SPRITE_TYPE_FOX_IDLE_SHADOW,
    WORLD_SPRITE_TYPE_FOX_RUN,
    WORLD_SPRITE_TYPE_FOX_RUN_SHADOW,
    WORLD_SPRITE_TYPE_BIRD_FLYING,
    WORLD_SPRITE_TYPE_COUNT,
} WorldSpriteType;

class GameStage {
  public:
    explicit GameStage(const std::shared_ptr<SDL_Renderer>& renderer,
                       const std::shared_ptr<SDL_Window>& window);

    // Not copyable or movable
    GameStage(const GameStage&) = delete;
    GameStage& operator=(const GameStage&) = delete;
    GameStage(GameStage&&) noexcept = delete;
    GameStage& operator=(GameStage&&) noexcept = delete;

    void processInput(const SDL_Event& event);
    void update(const Timer& timer);
    void render() const;

  private:
    void initSprites();
    void initWorld();
    void initAgents();

    void resolveViewportBoundary(Agent& agent) const;

    std::unordered_map<WorldSpriteType, Sprite> sprites_;
    AgentManager agents_;
    Physics physics_;
    Renderer renderer_;
    ViewportBounds viewport_bounds_;
    std::shared_ptr<SDL_Window> window_;
    PlayerAgent* player_ = nullptr;
    squirrel::Background background_;
};
