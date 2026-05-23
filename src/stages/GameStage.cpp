#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

#include "../agent/BirdAgent.h"
#include "../agent/PlayerAgent.h"
#include "GameStage.h"

GameStage::GameStage(const std::shared_ptr<SDL_Renderer>& renderer,
                     const std::shared_ptr<SDL_Window>& window)
    : renderer_(renderer), window_(window) {

    int w, h;
    SDL_GetWindowSizeInPixels(window_.get(), &w, &h);
    renderer_.setViewportSize(w, h);

    initWorld();
}

void GameStage::update(const Timer& timer) {
    for (auto& agent : agents_) {
        agent->update(timer);
        physics_.update(*agent, timer);

        agent->setIsGrounded(false);
        resolveViewportBoundary(*agent);

        agent->getSprite()->updateAnimationState(agent->getMutableAnimationState(), timer);
    }

    agents_.flushDeferred();

    // Scroll sky
    background_.sky_offset_x += 0.004f * timer.delta_time;
    if (background_.sky_offset_x > 1.0f) {
        background_.sky_offset_x -= 1.0f;
    }
}

void GameStage::render() const {
    renderer_.beginScene();
    renderer_.renderBackground(background_);
    for (const auto& agent : agents_) {
        renderer_.render(*agent);
    }
    renderer_.endScene();
}

void GameStage::resolveViewportBoundary(Agent& agent) const {
    auto size = agent.getSize();

    uint8_t edges = VIEWPORT_EDGE_NONE;
    if (agent.getPosition().x < viewport_bounds_.left) {
        edges |= VIEWPORT_EDGE_LEFT;

        if (agent.getPosition().x < viewport_bounds_.left - size.w) {
            edges |= VIEWPORT_OUTSIDE;
        }
    }

    if (agent.getPosition().x > viewport_bounds_.right - size.w) {
        edges |= VIEWPORT_EDGE_RIGHT;

        if (agent.getPosition().x > viewport_bounds_.right) {
            edges |= VIEWPORT_OUTSIDE;
        }
    }

    if (agent.getPosition().y < viewport_bounds_.top) {
        edges |= VIEWPORT_EDGE_TOP;

        if (agent.getPosition().y < viewport_bounds_.top - size.h) {
            edges |= VIEWPORT_OUTSIDE;
        }
    }

    if (agent.getPosition().y > viewport_bounds_.bottom) {
        edges |= VIEWPORT_EDGE_BOTTOM;

        if (agent.getPosition().y > viewport_bounds_.bottom + size.h) {
            edges |= VIEWPORT_OUTSIDE;
        }
    }

    if (edges != VIEWPORT_EDGE_NONE) {
        agent.onViewportBoundaryCollision(viewport_bounds_, edges);
    }
}

void GameStage::processInput(const SDL_Event& event) {
    player_->processInput(event);
}

void GameStage::initWorld() {
    viewport_bounds_ = {0.0f, 0.0f, 1.0f, 0.85f};

    background_.ground_texture_name = "ground";
    background_.trees_texture_name = "trees";
    background_.sky_texture_name = "sky";
    background_.sky_color = renderer_.getTextureColor("sky", 0, 323);

    renderer_.loadTexture(background_.ground_texture_name);
    renderer_.loadTexture(background_.trees_texture_name);
    renderer_.loadTexture(background_.sky_texture_name);

    initSprites();
    initAgents();
}

void GameStage::initSprites() {
    Sprite fox_idle_sprite(renderer_.loadTexture("fox_idle"), 32, 32);
    fox_idle_sprite.addAnimation(SPRITE_ANIMATION_FACE_FRONT, 0);
    fox_idle_sprite.addAnimation(SPRITE_ANIMATION_FACE_BACK, 1);
    fox_idle_sprite.addAnimation(SPRITE_ANIMATION_FACE_LEFT, 2);
    fox_idle_sprite.addAnimation(SPRITE_ANIMATION_FACE_RIGHT, 3);
    sprites_.insert({WORLD_SPRITE_TYPE_FOX_IDLE, std::move(fox_idle_sprite)});

    Sprite fox_idle_shadow_sprite(renderer_.loadTexture("fox_idle_shadow"), 32, 32);
    fox_idle_shadow_sprite.addAnimation(SPRITE_ANIMATION_FACE_FRONT, 0);
    fox_idle_shadow_sprite.addAnimation(SPRITE_ANIMATION_FACE_BACK, 1);
    fox_idle_shadow_sprite.addAnimation(SPRITE_ANIMATION_FACE_LEFT, 2);
    fox_idle_shadow_sprite.addAnimation(SPRITE_ANIMATION_FACE_RIGHT, 3);
    sprites_.insert({WORLD_SPRITE_TYPE_FOX_IDLE_SHADOW, std::move(fox_idle_shadow_sprite)});

    Sprite fox_run_sprite(renderer_.loadTexture("fox_run"), 32, 32);
    fox_run_sprite.addAnimation(SPRITE_ANIMATION_FACE_FRONT, 0);
    fox_run_sprite.addAnimation(SPRITE_ANIMATION_FACE_BACK, 1);
    fox_run_sprite.addAnimation(SPRITE_ANIMATION_FACE_RIGHT, 2);
    fox_run_sprite.addAnimation(SPRITE_ANIMATION_FACE_LEFT, 3);
    sprites_.insert({WORLD_SPRITE_TYPE_FOX_RUN, std::move(fox_run_sprite)});

    Sprite fox_run_shadow_sprite(renderer_.loadTexture("fox_run_shadow"), 32, 32);
    fox_run_shadow_sprite.addAnimation(SPRITE_ANIMATION_FACE_FRONT, 0);
    fox_run_shadow_sprite.addAnimation(SPRITE_ANIMATION_FACE_BACK, 1);
    fox_run_shadow_sprite.addAnimation(SPRITE_ANIMATION_FACE_RIGHT, 2);
    fox_run_shadow_sprite.addAnimation(SPRITE_ANIMATION_FACE_LEFT, 3);
    sprites_.insert({WORLD_SPRITE_TYPE_FOX_RUN_SHADOW, std::move(fox_run_shadow_sprite)});

    Sprite bird_fly_sprite(renderer_.loadTexture("bird_fly"), 32, 32);
    bird_fly_sprite.addAnimation(SPRITE_ANIMATION_FACE_FRONT, 0);
    bird_fly_sprite.addAnimation(SPRITE_ANIMATION_FACE_BACK, 1);
    bird_fly_sprite.addAnimation(SPRITE_ANIMATION_FACE_LEFT, 2);
    bird_fly_sprite.addAnimation(SPRITE_ANIMATION_FACE_RIGHT, 3);
    sprites_.insert({WORLD_SPRITE_TYPE_BIRD_FLYING, std::move(bird_fly_sprite)});
}

void GameStage::initAgents() {
    player_ = nullptr;

    auto player = std::make_unique<PlayerAgent>(
        squirrel::Vector2f{(viewport_bounds_.right - 0.05f) / 2.0f, viewport_bounds_.bottom},
        SpriteAnimationState{.face = SPRITE_ANIMATION_FACE_FRONT, .fps = 4.0f},
        &sprites_.at(WORLD_SPRITE_TYPE_FOX_IDLE),
        &sprites_.at(WORLD_SPRITE_TYPE_FOX_IDLE_SHADOW),
        &sprites_.at(WORLD_SPRITE_TYPE_FOX_RUN),
        &sprites_.at(WORLD_SPRITE_TYPE_FOX_RUN_SHADOW),
        physics_);
    player_ = player.get();
    agents_.add(std::move(player));

    for (int i = 0; i < 2; i += 1) {
        auto bird = std::make_unique<BirdAgent>(
            squirrel::Vector2f{0, 0},
            SpriteAnimationState{.face = SPRITE_ANIMATION_FACE_RIGHT, .fps = 8.0f},
            &sprites_.at(WORLD_SPRITE_TYPE_BIRD_FLYING));
        bird->reset(viewport_bounds_);
        agents_.add(std::move(bird));
    }
}
