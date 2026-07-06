#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

#include "../agent/BirdAgent.h"
#include "../agent/PlayerAgent.h"
#include "GameStage.h"

GameStage::GameStage(SDL_Renderer* renderer, SDL_Window* window)
    : renderer_(renderer), window_(window) {

    int w, h;
    SDL_GetWindowSizeInPixels(window_, &w, &h);
    renderer_.setViewportSize(w, h);

    initWorld();
}

void GameStage::update(const Timer& timer) {
    for (auto& agent : agents_) {
        agent->update(timer, agents_);
        physics_.update(*agent, timer);
        collision_map_.update(*agent);

        agent->setIsGrounded(false);

        resolveViewportBoundary(*agent);
        for (auto& collision_agent : collision_map_.getCollidingAgents(*agent)) {
            agent->onAgentCollision(*collision_agent, agents_);
        }

        agent->getSprite()->updateAnimationState(agent->getMutableAnimationState(), timer);
    }

    agents_.flushDeferred([&](Agent& agent) { collision_map_.remove(agent); });

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

    ViewportEdge edges = ViewportEdge::None;
    if (agent.getPosition().x < viewport_bounds_.left) {
        edges |= ViewportEdge::Left;

        if (agent.getPosition().x < viewport_bounds_.left - size.w) {
            edges |= ViewportEdge::Outside;
        }
    }

    if (agent.getPosition().x > viewport_bounds_.right - size.w) {
        edges |= ViewportEdge::Right;

        if (agent.getPosition().x > viewport_bounds_.right) {
            edges |= ViewportEdge::Outside;
        }
    }

    if (agent.getPosition().y < viewport_bounds_.top) {
        edges |= ViewportEdge::Top;

        if (agent.getPosition().y < viewport_bounds_.top - size.h) {
            edges |= ViewportEdge::Outside;
        }
    }

    if (agent.getPosition().y + size.h > viewport_bounds_.bottom) {
        edges |= ViewportEdge::Bottom;

        if (agent.getPosition().y > viewport_bounds_.bottom) {
            edges |= ViewportEdge::Outside;
        }
    }

    if (edges != ViewportEdge::None) {
        agent.onViewportBoundaryCollision(viewport_bounds_, edges);
    }
}

void GameStage::processInput(const SDL_Event& event) {
    player_->processInput(event);
}

void GameStage::initWorld() {
    viewport_bounds_ = {0.0f, 0.0f, 1.0f, 0.95f};

    background_.ground_texture_name = "ground";
    background_.trees_texture_name = "trees";
    background_.sky_texture_name = "sky";
    background_.sky_color = renderer_.getTextureColor("sky", 0, 323);

    (void)renderer_.loadTexture(background_.ground_texture_name);
    (void)renderer_.loadTexture(background_.trees_texture_name);
    (void)renderer_.loadTexture(background_.sky_texture_name);

    initSprites();
    initAgents();
}

void GameStage::initSprites() {
    Sprite fox_idle_sprite(renderer_.loadTexture("fox_idle"), 32, 32);
    fox_idle_sprite.addAnimation(SpriteAnimationFace::Front, 0);
    fox_idle_sprite.addAnimation(SpriteAnimationFace::Back, 1);
    fox_idle_sprite.addAnimation(SpriteAnimationFace::Left, 2);
    fox_idle_sprite.addAnimation(SpriteAnimationFace::Right, 3);
    sprites_.insert({WorldSpriteType::FoxIdle, std::move(fox_idle_sprite)});

    Sprite fox_idle_shadow_sprite(renderer_.loadTexture("fox_idle_shadow"), 32, 32);
    fox_idle_shadow_sprite.addAnimation(SpriteAnimationFace::Front, 0);
    fox_idle_shadow_sprite.addAnimation(SpriteAnimationFace::Back, 1);
    fox_idle_shadow_sprite.addAnimation(SpriteAnimationFace::Left, 2);
    fox_idle_shadow_sprite.addAnimation(SpriteAnimationFace::Right, 3);
    sprites_.insert({WorldSpriteType::FoxIdleShadow, std::move(fox_idle_shadow_sprite)});

    Sprite fox_run_sprite(renderer_.loadTexture("fox_run"), 32, 32);
    fox_run_sprite.addAnimation(SpriteAnimationFace::Front, 0);
    fox_run_sprite.addAnimation(SpriteAnimationFace::Back, 1);
    fox_run_sprite.addAnimation(SpriteAnimationFace::Right, 2);
    fox_run_sprite.addAnimation(SpriteAnimationFace::Left, 3);
    sprites_.insert({WorldSpriteType::FoxRun, std::move(fox_run_sprite)});

    Sprite fox_run_shadow_sprite(renderer_.loadTexture("fox_run_shadow"), 32, 32);
    fox_run_shadow_sprite.addAnimation(SpriteAnimationFace::Front, 0);
    fox_run_shadow_sprite.addAnimation(SpriteAnimationFace::Back, 1);
    fox_run_shadow_sprite.addAnimation(SpriteAnimationFace::Right, 2);
    fox_run_shadow_sprite.addAnimation(SpriteAnimationFace::Left, 3);
    sprites_.insert({WorldSpriteType::FoxRunShadow, std::move(fox_run_shadow_sprite)});

    Sprite bird_fly_sprite(renderer_.loadTexture("bird_fly"), 32, 32);
    bird_fly_sprite.addAnimation(SpriteAnimationFace::Front, 0);
    bird_fly_sprite.addAnimation(SpriteAnimationFace::Back, 1);
    bird_fly_sprite.addAnimation(SpriteAnimationFace::Left, 2);
    bird_fly_sprite.addAnimation(SpriteAnimationFace::Right, 3);
    sprites_.insert({WorldSpriteType::BirdFlying, std::move(bird_fly_sprite)});

    Sprite egg_sprite(renderer_.loadTexture("bird_egg"), 32, 32);
    sprites_.insert({WorldSpriteType::Egg, std::move(egg_sprite)});
}

void GameStage::initAgents() {
    player_ = nullptr;

    auto player = std::make_unique<PlayerAgent>(
        squirrel::Vector2f{(viewport_bounds_.right - 0.05f) / 2.0f, 0.5f},
        SpriteAnimationState{.face = SpriteAnimationFace::Front, .fps = 4.0f},
        &sprites_.at(WorldSpriteType::FoxIdle),
        &sprites_.at(WorldSpriteType::FoxIdleShadow),
        &sprites_.at(WorldSpriteType::FoxRun),
        &sprites_.at(WorldSpriteType::FoxRunShadow),
        physics_);
    player_ = player.get();
    agents_.add(std::move(player));

    for (int i = 0; i < 2; i += 1) {
        auto bird = std::make_unique<BirdAgent>(
            squirrel::Vector2f{0, 0},
            SpriteAnimationState{.face = SpriteAnimationFace::Right, .fps = 8.0f},
            &sprites_.at(WorldSpriteType::BirdFlying),
            &sprites_.at(WorldSpriteType::Egg));
        bird->reset(viewport_bounds_);
        agents_.add(std::move(bird));
    }
}
