#pragma once

#include <SDL3/SDL.h>
#include <flat_map>

#include "../agent/Agent.h"
#include "../agent/AgentManager.h"
#include "../physics/CollisionZone.h"
#include "../physics/Physics.h"
#include "../renderer/Background.h"
#include "../renderer/Renderer.h"
#include "../sprites/Sprite.h"
#include "../utils/Timer.h"

class PlayerAgent;

enum class WorldSpriteType : uint8_t {
    FoxIdle,
    FoxIdleShadow,
    FoxRun,
    FoxRunShadow,
    BirdFlying,
    Egg,
    Count,
};

static constexpr size_t kCollisionGridColumns = 12;
static constexpr size_t kCollisionGridRows = 8;

class GameStage {
  public:
    explicit GameStage(SDL_Renderer* renderer, SDL_Window* window);
    ~GameStage() = default;

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

    std::flat_map<WorldSpriteType, Sprite> sprites_;
    AgentManager agents_;
    Physics physics_;
    CollisionZone collision_map_{kCollisionGridRows, kCollisionGridColumns};
    Renderer renderer_;
    ViewportBounds viewport_bounds_;
    SDL_Window* window_;
    PlayerAgent* player_ = nullptr;
    squirrel::Background background_;
};
