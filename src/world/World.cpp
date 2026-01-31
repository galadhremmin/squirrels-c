#include <SDL3/SDL.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

#include "../agent/Agent.h"
#include "../agent/AgentState.h"
#include "World.h"

World::World(const std::shared_ptr<SDL_Renderer>& renderer,
             const std::shared_ptr<SDL_Window>& window)
    : renderer_(renderer), window_(window), player_agent_index_(SIZE_MAX) {

    int w, h;
    SDL_GetWindowSizeInPixels(window_.get(), &w, &h);
    renderer_.setViewportSize(w, h);

    initWorld();
}

void World::update(const Timer& timer) {
    auto& player_agent = agents_.at(player_agent_index_);
    switch (player_agent_state_machine_->update(timer)) {
    case AGENT_STATE_IDLE:
        player_agent.sprite_ = &sprites_.at(WORLD_SPRITE_TYPE_FOX_IDLE);
        Sprite::resetAnimationState(player_agent.animation_state_, SPRITE_ANIMATION_FACE_FRONT);
        player_agent.velocity_x_ = 0.0f;
        player_agent.velocity_y_ = 0.0f;
        player_agent.direction_ = 0.0f;

        break;
    case AGENT_STATE_RUN_LEFT:
        player_agent.sprite_ = &sprites_.at(WORLD_SPRITE_TYPE_FOX_RUN);
        Sprite::resetAnimationState(player_agent.animation_state_, SPRITE_ANIMATION_FACE_LEFT);
        player_agent.velocity_x_ = -75.0f;
        break;
    case AGENT_STATE_RUN_RIGHT:
        player_agent.sprite_ = &sprites_.at(WORLD_SPRITE_TYPE_FOX_RUN);
        Sprite::resetAnimationState(player_agent.animation_state_, SPRITE_ANIMATION_FACE_RIGHT);
        player_agent.velocity_x_ = 75.0f;
        break;
    case AGENT_STATE_JUMP:
        if (player_agent_state_machine_->getPreviousStateId() == AGENT_STATE_RUN_LEFT) {
            Sprite::resetAnimationState(player_agent.animation_state_, SPRITE_ANIMATION_FACE_LEFT);
        } else if (player_agent_state_machine_->getPreviousStateId() == AGENT_STATE_RUN_RIGHT) {
            Sprite::resetAnimationState(player_agent.animation_state_, SPRITE_ANIMATION_FACE_RIGHT);
        } else if (player_agent_state_machine_->getPreviousStateId() == AGENT_STATE_IDLE) {
            Sprite::resetAnimationState(player_agent.animation_state_, SPRITE_ANIMATION_FACE_FRONT);
        }

        player_agent.velocity_x_ = 0.0f;
        player_agent.velocity_y_ = -50.0f;
        player_agent.direction_ = 0.0f;
        break;
    case AGENT_STATE_DYING:
        break;
    case AGENT_STATE_DEAD:
        break;
    default:
        break;
    }

    for (auto& agent : agents_) {
        agent.sprite_->updateAnimationState(agent.animation_state_, timer);
        agent.position_x_ += agent.velocity_x_ * timer.delta_time;
        agent.position_y_ += agent.velocity_y_ * timer.delta_time;
    }

    background_.sky_offset_x += 5.0f * timer.delta_time;
    if (background_.sky_offset_x > renderer_.getViewportSize().w) {
        background_.sky_offset_x -= renderer_.getViewportSize().w;
    }

    renderer_.beginScene();

    renderer_.renderBackground(background_);

    for (const auto& agent : agents_) {
        renderer_.render(agent);
    }

    renderer_.endScene();
}

void World::processInput(const SDL_Event& event) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
        if (event.key.repeat) {
            break;
        }
        switch (event.key.scancode) {
        case SDL_SCANCODE_LEFT:
            player_agent_state_machine_->pushState(AGENT_STATE_RUN_LEFT);
            break;
        case SDL_SCANCODE_RIGHT:
            player_agent_state_machine_->pushState(AGENT_STATE_RUN_RIGHT);
            break;
        case SDL_SCANCODE_SPACE:
            player_agent_state_machine_->pushState(AGENT_STATE_JUMP);
            break;
        case SDL_SCANCODE_DOWN:
            player_agent_state_machine_->pushState(
                AGENT_STATE_DYING); // temporary, just for testing
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
            player_agent_state_machine_->pushState(AGENT_STATE_IDLE);
            break;
        default:
            // noop, do nothing
            break;
        }
    }
}

void World::initWorld() {
    background_.ground_texture_name = "ground";
    background_.sky_texture_name = "sky";
    background_.sky_color = renderer_.getTextureColor("sky", 0, 323);

    initSprites();
    initAgents();
    initStateMachines();
}

void World::initSprites() {
    Sprite fox_idle_sprite(renderer_.loadTexture("fox_idle"), 32, 32);
    fox_idle_sprite.addAnimation(SPRITE_ANIMATION_FACE_FRONT, 0);
    fox_idle_sprite.addAnimation(SPRITE_ANIMATION_FACE_BACK, 1);
    fox_idle_sprite.addAnimation(SPRITE_ANIMATION_FACE_LEFT, 2);
    fox_idle_sprite.addAnimation(SPRITE_ANIMATION_FACE_RIGHT, 3);
    sprites_.insert({WORLD_SPRITE_TYPE_FOX_IDLE, std::move(fox_idle_sprite)});

    Sprite fox_run_sprite(renderer_.loadTexture("fox_run"), 32, 32);
    fox_run_sprite.addAnimation(SPRITE_ANIMATION_FACE_FRONT, 0);
    fox_run_sprite.addAnimation(SPRITE_ANIMATION_FACE_BACK, 1);
    fox_run_sprite.addAnimation(SPRITE_ANIMATION_FACE_RIGHT, 2);
    fox_run_sprite.addAnimation(SPRITE_ANIMATION_FACE_LEFT, 3);
    sprites_.insert({WORLD_SPRITE_TYPE_FOX_RUN, std::move(fox_run_sprite)});
}

void World::initAgents() {
    agents_.clear();

    int w, h;
    SDL_GetWindowSizeInPixels(window_.get(), &w, &h);

    // Create player agent
    Agent player{"player"};

    player.sprite_ = &sprites_.at(WORLD_SPRITE_TYPE_FOX_IDLE);
    player.animation_state_ = SpriteAnimationState{SPRITE_ANIMATION_FACE_RIGHT, 0, 4.0f, 0};
    player.position_x_ = (w - 32) / 2.0f;
    player.position_y_ = (h - 64 * 1.5);

    agents_.push_back(std::move(player));
    player_agent_index_ = 0;
}

void World::initStateMachines() {
    player_agent_state_machine_ = std::make_unique<StateMachine>(
        StateProvider({
            {AGENT_STATE_IDLE, {true, true, false, 0, AGENT_STATE_IDLE}},
            {AGENT_STATE_RUN_LEFT, {true, true, false, 0, AGENT_STATE_IDLE}},
            {AGENT_STATE_RUN_RIGHT, {true, true, false, 0, AGENT_STATE_IDLE}},
            {AGENT_STATE_JUMP, {true, false, true, 2'000'000'000, AGENT_STATE_IDLE}},
            {AGENT_STATE_DYING, {false, false, true, 2'000'000'000, AGENT_STATE_DEAD}},
            {AGENT_STATE_DEAD, {false, false, false, 0, AGENT_STATE_DEAD}},
        }),
        AGENT_STATE_IDLE);
}
