#pragma once

#include <SDL3/SDL.h>
#include <memory>

#include "../agent/Agent.h"
#include "../sprites/Sprite.h"
#include "../utils/Timer.h"

class Renderer {
  public:
    explicit Renderer(const std::shared_ptr<SDL_Renderer>& renderer);

    void begin_scene() const;
    void end_scene() const;
    void render(const Agent& agent, const Timer& timer);
    const std::shared_ptr<SDL_Renderer>& get_renderer() const {
        return renderer_;
    }

  private:
    void render_sprite(const Sprite& sprite,
                       const SpriteAnimationFace face,
                       const uint8_t frame_number,
                       const SDL_FRect& dst_rect);

    const std::shared_ptr<SDL_Renderer>& renderer_;
};
