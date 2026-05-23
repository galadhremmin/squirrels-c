#pragma once

#include <SDL3/SDL.h>
#include <memory>

#include "../agent/Agent.h"
#include "../sprites/Sprite.h"
#include "../utils/Timer.h"
#include "Background.h"
#include "Texture.h"

class Renderer {
  public:
    explicit Renderer(const std::shared_ptr<SDL_Renderer> renderer);

    // Not copyable or movable.
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;

    const squirrel::Texture* loadTexture(const std::string& name);
    const squirrel::Texture* getLoadedTexture(const std::string& name) const;
    SDL_Color getTextureColor(const std::string& name, const uint32_t x, const uint32_t y);

    void setViewportSize(const int width, const int height);
    void beginScene() const;
    void endScene() const;

    void render(const Agent& agent) const;
    void renderBackground(const squirrel::Background& background) const;

  private:
    void renderSprite(const Sprite& sprite,
                      const SpriteAnimationFace face,
                      const uint8_t frame_number,
                      const SDL_FRect& dst_rect) const;

    const squirrel::Texture* getLoadedTextureUnchecked(const std::string& name) const;
    static void freeLoadedTexture(squirrel::Texture* texture);

    const std::shared_ptr<SDL_Renderer> renderer_;
    std::unordered_map<std::string,
                       std::unique_ptr<squirrel::Texture, decltype(&freeLoadedTexture)>>
        textures_;
    SDL_Rect viewport_size_;
};
