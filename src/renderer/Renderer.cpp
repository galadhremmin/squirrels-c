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

void Renderer::beginScene() const {
    SDL_SetRenderDrawColor(renderer_.get(), 30, 60, 90, 255);
    SDL_RenderClear(renderer_.get());
}

void Renderer::endScene() const {
    SDL_RenderPresent(renderer_.get());
}

void Renderer::render(const Agent& agent, const Timer& timer) {
    if (agent.sprite_ == nullptr) {
        return;
    }

    SDL_FRect dst_rect = {
        .x = agent.position_x_,
        .y = agent.position_y_,
        .w = static_cast<float>(agent.sprite_->getFrameWidth()) * 2.0f,
        .h = static_cast<float>(agent.sprite_->getFrameHeight()) * 2.0f,
    };

    renderSprite(
        *agent.sprite_, agent.animation_state_.face, agent.animation_state_.frame_number, dst_rect);
}

void Renderer::renderSprite(const Sprite& sprite,
                            const SpriteAnimationFace face,
                            const uint8_t frame_number,
                            const SDL_FRect& dst_rect) {
    SDL_FRect src_rect = {
        .x = static_cast<float>(frame_number * sprite.getFrameWidth()),
        .y = static_cast<float>(sprite.getOffsetIndex(face) * sprite.getFrameHeight()),
        .w = static_cast<float>(sprite.getFrameWidth()),
        .h = static_cast<float>(sprite.getFrameHeight()),
    };

    SDL_RenderTexture(renderer_.get(), sprite.getTexturePtr(), &src_rect, &dst_rect);
}
