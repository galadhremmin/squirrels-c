#include "Renderer.h"

#include <stdexcept>

Renderer::Renderer(const std::shared_ptr<SDL_Renderer>& renderer) : renderer_(renderer) {
    if (renderer == nullptr) {
        throw std::runtime_error("Renderer must not be null");
    }

    const int enable_vsync = 1;
    if (!SDL_SetRenderVSync(renderer_.get(), enable_vsync)) {
        SDL_Log("VSync set to %d failed: %s", enable_vsync, SDL_GetError());
    }
}

void Renderer::begin_scene() const {
    SDL_SetRenderDrawColor(renderer_.get(), 30, 60, 90, 255);
    SDL_RenderClear(renderer_.get());
}

void Renderer::end_scene() const {
    SDL_RenderPresent(renderer_.get());
}

void Renderer::render(const Agent& agent, const Timer& timer) {
    if (agent.sprite_ == nullptr) {
        return;
    }

    SDL_FRect dst_rect = {
        .x = agent.position_x_,
        .y = agent.position_y_,
        .w = static_cast<float>(agent.sprite_->frame_width()) * 2.0f,
        .h = static_cast<float>(agent.sprite_->frame_height()) * 2.0f,
    };

    render_sprite(
        *agent.sprite_, agent.animation_state_.face, agent.animation_state_.frame_number, dst_rect);
}

void Renderer::render_sprite(const Sprite& sprite,
                             const SpriteAnimationFace face,
                             const uint8_t frame_number,
                             const SDL_FRect& dst_rect) {
    SDL_FRect src_rect = {
        .x = static_cast<float>(frame_number * sprite.frame_width()),
        .y = static_cast<float>(sprite.get_offset_index(face) * sprite.frame_height()),
        .w = static_cast<float>(sprite.frame_width()),
        .h = static_cast<float>(sprite.frame_height()),
    };

    SDL_RenderTexture(renderer_.get(), sprite.texture(), &src_rect, &dst_rect);
}
