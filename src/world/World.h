#pragma once
#include <SDL3/SDL.h>

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../agent/Agent.h"
#include "../renderer/Renderer.h"
#include "../sprites/Sprite.h"
#include "../utils/Timer.h"

typedef enum {
    WORLD_SPRITE_TYPE_FOX_IDLE,
    WORLD_SPRITE_TYPE_FOX_RUN,
    WORLD_SPRITE_TYPE_COUNT,
} WorldSpriteType;

class World {
  public:
    explicit World(const std::shared_ptr<SDL_Renderer>& renderer);

    // Not copyable or movable
    World(const World&) = delete;
    World& operator=(const World&) = delete;
    World(World&&) noexcept = delete;
    World& operator=(World&&) noexcept = delete;

    void processInput(const SDL_Event& event);
    void update(const Timer& timer);
    void render(const Timer& timer);

  private:
    bool initSprites();
    bool initAgents();
    bool initStateMachines();

    std::unordered_map<WorldSpriteType, Sprite> sprites_;
    std::vector<Agent> agents_;
    Renderer renderer_;
    size_t player_agent_index_;
    std::unique_ptr<StateMachine> player_agent_state_machine_;
};
