#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>

#include "../agent/Agent.h"
#include "../agent/AgentState.h"
#include "World.h"

static constexpr float kMoveSpeed = 150.0f;

World::World(const std::shared_ptr<SDL_Renderer>& renderer,
             const std::shared_ptr<SDL_Window>& window)
    : renderer_(renderer), window_(window), player_agent_index_(SIZE_MAX) {

    int w, h;
    SDL_GetWindowSizeInPixels(window_.get(), &w, &h);
    renderer_.setViewportSize(w, h);

    initWorld();
}

void World::update(const Timer& timer) {
    auto& player = agents_.at(player_agent_index_);

    // Phase 1: Apply input to horizontal velocity; gate jump on is_grounded
    if (input_left_) {
        player.getMutableVelocity().x = -kMoveSpeed;
    } else if (input_right_) {
        player.getMutableVelocity().x = kMoveSpeed;
    } else {
        player.getMutableVelocity().x = 0.0f;
    }

    if (input_jump_ && player.getIsGrounded()) {
        physics_.applyJumpImpulse(player);
    }

    // Phase 2: Integrate physics, then resolve collision
    physics_.step(player, timer);
    player.setIsGrounded(resolveCollision(player));

    // Phase 3: Derive animation sprite/face from physics state (only resets on change)
    updatePlayerAnimation(player);

    // Advance animation frames for all agents
    for (auto& agent : agents_) {
        agent.getSprite()->updateAnimationState(agent.getMutableAnimationState(), timer);
    }

    // Scroll sky
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

bool World::resolveCollision(Agent& agent) const {
    if (agent.getPosition().y >= ground_y_) {
        agent.getMutablePosition().y = ground_y_;
        agent.getMutableVelocity().y = 0.0f;
        return true;
    }
    return false;
}

void World::updatePlayerAnimation(Agent& agent) {
    AgentStateId anim_state;
    SpriteAnimationFace face;

    if (agent.getVelocity().x < 0.0f) {
        anim_state = AGENT_STATE_RUNNING;
        face = SPRITE_ANIMATION_FACE_LEFT;
    } else if (agent.getVelocity().x > 0.0f) {
        anim_state = AGENT_STATE_RUNNING;
        face = SPRITE_ANIMATION_FACE_RIGHT;
    } else {
        anim_state = AGENT_STATE_IDLE;
        face = SPRITE_ANIMATION_FACE_FRONT;
    }

    if (anim_state != prev_anim_state_ || face != prev_face_) {
        Sprite* next_sprite = (anim_state == AGENT_STATE_RUNNING)
                                  ? &sprites_.at(WORLD_SPRITE_TYPE_FOX_RUN)
                                  : &sprites_.at(WORLD_SPRITE_TYPE_FOX_IDLE);
        if (agent.getSprite() != next_sprite) {
            agent.setSprite(next_sprite);
        }
        Sprite::resetAnimationState(agent.getMutableAnimationState(), face);
        prev_anim_state_ = anim_state;
        prev_face_ = face;
    }
}

void World::processInput(const SDL_Event& event) {
    const bool keydown = (event.type == SDL_EVENT_KEY_DOWN);
    const bool keyup = (event.type == SDL_EVENT_KEY_UP);

    if ((!keydown && !keyup) || event.key.repeat)
        return;

    switch (event.key.scancode) {
    case SDL_SCANCODE_LEFT:
    case SDL_SCANCODE_A:
        input_left_ = keydown;
        break;
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_D:
        input_right_ = keydown;
        break;
    case SDL_SCANCODE_SPACE:
        input_jump_ = keydown;
        break;
    default:
        break;
    }
}

void World::initWorld() {
    background_.ground_texture_name = "ground";
    background_.sky_texture_name = "sky";
    background_.sky_color = renderer_.getTextureColor("sky", 0, 323);

    initSprites();
    initAgents();
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

    Agent player{"player",
                 squirrel::Vector2f{(w - 32.0f) / 2.0f, (h - 64.0f * 1.5f)},
                 SpriteAnimationState{
                     .face = SPRITE_ANIMATION_FACE_FRONT,
                     .fps = 4.0f,
                 }};
    player.setSprite(&sprites_.at(WORLD_SPRITE_TYPE_FOX_IDLE));

    ground_y_ = player.getPosition().y;

    agents_.push_back(std::move(player));
    player_agent_index_ = 0;
}
