#include <SDL3/SDL.h>
#include <SDL3/SDL_scancode.h>
#include <cstddef>
#include <stdexcept>

#include "../agent/Agent.h"
#include "../agent/AgentState.h"
#include "World.h"

World::World(const std::shared_ptr<SDL_Renderer>& renderer)
    : renderer_(renderer), player_agent_index_(SIZE_MAX) {
    if (!init_sprites()) {
        throw std::runtime_error("Failed to initialize world sprites");
    }
    if (!init_agents()) {
        throw std::runtime_error("Failed to initialize world agents");
    }
}

void World::update(const Timer& timer) {
    for (auto& agent : agents_) {
        /*
        int new_state_id = agent_state_update(agent, &timer);
        if (new_state_id != -1) {
            switch (static_cast<AgentStateId>(new_state_id)) {
            case AGENT_STATE_IDLE:
                agent.sprite_ = sprites_[WORLD_SPRITE_TYPE_FOX_IDLE].get();
                break;
            case AGENT_STATE_RUN_LEFT:
            case AGENT_STATE_RUN_RIGHT:
                agent.sprite_ = sprites_[WORLD_SPRITE_TYPE_FOX_RUN].get();
                break;
            default:
                break;
            }
        }
        */
        agent.sprite_->update_animation_state(agent.animation_state_, timer);

        agent.position_x_ += agent.velocity_x_ * timer.delta_time;
        agent.position_y_ += agent.velocity_y_ * timer.delta_time;
    }
}

void World::process_input(const SDL_Event& event) {
    /*
    auto& player_agent = agents_.at(player_agent_index_);
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
        if (event.key.repeat) {
            break;
        }
        switch (event.key.scancode) {
        case SDL_SCANCODE_LEFT:
            agent_state_push(player_agent, AGENT_STATE_RUN_LEFT);
            break;
        case SDL_SCANCODE_RIGHT:
            agent_state_push(player_agent, AGENT_STATE_RUN_RIGHT);
            break;
        case SDL_SCANCODE_SPACE:
            agent_state_push(player_agent, AGENT_STATE_JUMP);
            break;
        case SDL_SCANCODE_DOWN:
            agent_state_push(player_agent, AGENT_STATE_DYING); // temporary, just for testing
            break;
        default:
            // noop, do nothing
            break;
        }
        break;
    case SDL_EVENT_KEY_UP:
        if (event.key.repeat) {
            break;
        }
        switch (event.key.scancode) {
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_SPACE:
            agent_state_push(player_agent, AGENT_STATE_IDLE);
            break;
        default:
            // noop, do nothing
            break;
        }
    }
    */
}

bool World::init_sprites() {
    try {
        auto r = renderer_.get_renderer();

        Sprite fox_idle_sprite(32, 32);
        fox_idle_sprite.load(r, "fox_idle");
        fox_idle_sprite.add_animation(SPRITE_ANIMATION_FACE_FRONT, 0);
        fox_idle_sprite.add_animation(SPRITE_ANIMATION_FACE_BACK, 1);
        fox_idle_sprite.add_animation(SPRITE_ANIMATION_FACE_LEFT, 2);
        fox_idle_sprite.add_animation(SPRITE_ANIMATION_FACE_RIGHT, 3);
        sprites_.insert({WORLD_SPRITE_TYPE_FOX_IDLE, std::move(fox_idle_sprite)});

        Sprite fox_run_sprite(32, 32);
        fox_run_sprite.load(r, "fox_run");
        fox_run_sprite.add_animation(SPRITE_ANIMATION_FACE_FRONT, 0);
        fox_run_sprite.add_animation(SPRITE_ANIMATION_FACE_BACK, 1);
        fox_run_sprite.add_animation(SPRITE_ANIMATION_FACE_RIGHT, 2);
        fox_run_sprite.add_animation(SPRITE_ANIMATION_FACE_LEFT, 3);
        sprites_.insert({WORLD_SPRITE_TYPE_FOX_RUN, std::move(fox_run_sprite)});

        return true;
    } catch (const std::exception& e) {
        SDL_Log("Failed to initialize sprites: %s", e.what());
        return false;
    }
}

bool World::init_agents() {
    agents_.clear();

    // Create player agent
    Agent player{"player"};

    player.sprite_ = &sprites_.at(WORLD_SPRITE_TYPE_FOX_IDLE);
    player.animation_state_ = AnimationState{SPRITE_ANIMATION_FACE_RIGHT, 0, 4.0f, 0};
    player.position_x_ = 100;
    player.position_y_ = 50;

    agent_state_init(&player);

    agents_.push_back(std::move(player));
    player_agent_index_ = 0;

    return true;
}

void World::render(const Timer& timer) {
    renderer_.begin_scene();

    for (const auto& agent : agents_) {
        renderer_.render(agent, timer);
    }

    renderer_.end_scene();
}
