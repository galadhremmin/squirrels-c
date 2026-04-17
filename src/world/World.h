#pragma once
#include <SDL3/SDL.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "../agent/Agent.h"
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

class World {
  public:
    explicit World(const std::shared_ptr<SDL_Renderer>& renderer,
                   const std::shared_ptr<SDL_Window>& window);

    // Not copyable or movable
    World(const World&) = delete;
    World& operator=(const World&) = delete;
    World(World&&) noexcept = delete;
    World& operator=(World&&) noexcept = delete;

    void processInput(const SDL_Event& event);
    void update(const Timer& timer);

  private:
    void initSprites();
    void initWorld();
    void initAgents();

    void resolveViewportBoundary(Agent& agent) const;

    std::unordered_map<WorldSpriteType, Sprite> sprites_;
    std::vector<std::unique_ptr<Agent>> agents_;
    Physics physics_;
    Renderer renderer_;
    ViewportBounds viewport_bounds_;
    std::shared_ptr<SDL_Window> window_;
    PlayerAgent* player_ = nullptr;
    squirrel::Background background_;
    float ground_y_;
};
