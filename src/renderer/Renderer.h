#pragma once

#include <SDL3/SDL.h>
#include <memory>

#include "../agent/Agent.h"
#include "../sprites/Sprite.h"
#include "../utils/Timer.h"

class Renderer {
  public:
    explicit Renderer(const std::shared_ptr<SDL_Renderer>& renderer);

    void beginScene() const;
    void endScene() const;
    void render(const Agent& agent, const Timer& timer);
    SDL_Renderer* getSdlRendererPtr() const {
        return renderer_.get();
    }

  private:
    void renderSprite(const Sprite& sprite,
                      const SpriteAnimationFace face,
                      const uint8_t frame_number,
                      const SDL_FRect& dst_rect);

    const std::shared_ptr<SDL_Renderer>& renderer_;
};
