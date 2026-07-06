#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <unordered_map>

#include "../agent/Agent.h"
#include "../sprites/Sprite.h"
#include "Background.h"
#include "Texture.h"

class Renderer {
  public:
    explicit Renderer(SDL_Renderer* renderer);

    // Not copyable or movable.
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;

    [[nodiscard]] const squirrel::Texture* loadTexture(const std::string& name);
    [[nodiscard]] const squirrel::Texture* getLoadedTexture(const std::string& name) const;
    [[nodiscard]] SDL_Color getTextureColor(const std::string& name, uint32_t x, uint32_t y);

    void setViewportSize(int width, int height);
    void beginScene() const;
    void endScene() const;

    void render(const Agent& agent) const;
    void renderBackground(const squirrel::Background& background) const;

    inline void setRenderBoundaryBoxes(bool b) {
        render_boundary_boxes_ = b;
    }

  private:
    void renderSprite(const Sprite& sprite,
                      SpriteAnimationFace face,
                      uint8_t frame_number,
                      const SDL_FRect& dst_rect) const;

    [[nodiscard]] const squirrel::Texture* getLoadedTextureUnchecked(const std::string& name) const;
    static void freeLoadedTexture(squirrel::Texture* texture);

    SDL_Renderer* const renderer_;
    std::unordered_map<std::string,
                       std::unique_ptr<squirrel::Texture, decltype(&freeLoadedTexture)>>
        textures_;
    SDL_Rect viewport_size_;

    bool render_boundary_boxes_{false};
};
