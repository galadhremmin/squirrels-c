#include "Renderer.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <stdexcept>

Renderer::Renderer(const std::shared_ptr<SDL_Renderer> renderer) : renderer_(renderer) {
    if (renderer == nullptr) {
        throw std::runtime_error("Renderer must not be null");
    }

    const int enable_vsync = 1;
    if (!SDL_SetRenderVSync(renderer_.get(), enable_vsync)) {
        SDL_Log("VSync set to %d failed: %s", enable_vsync, SDL_GetError());
    }
}

const squirrel::Texture* Renderer::loadTexture(const std::string& name) {
    // I'm not using contains here because I want to avoid an extra lookup for textures already
    // loaded.
    auto existing_texture = textures_.find(name);
    if (existing_texture != textures_.end()) {
        return existing_texture->second.get();
    }

    std::string path = "assets/textures/" + name + ".png";

    SDL_Texture* texture = IMG_LoadTexture(renderer_.get(), path.c_str());
    if (texture == nullptr) {
        SDL_Log("Failed to load texture: %s", path.c_str());
        throw std::runtime_error("Failed to load texture");
    }

    float tex_w = 0, tex_h = 0;
    if (!SDL_GetTextureSize(texture, &tex_w, &tex_h)) {
        SDL_Log("Failed to query texture: %s", path.c_str());
        throw std::runtime_error("Failed to query texture " + path);
    }

    SDL_Log("Loaded texture %s named %s (%p)",
            path.c_str(),
            name.c_str(),
            reinterpret_cast<void*>(texture));

    auto ptr = std::unique_ptr<squirrel::Texture, decltype(&Renderer::freeLoadedTexture)>(
        new squirrel::Texture{
            .texture = texture,
            .width = tex_w,
            .height = tex_h,
        },
        &Renderer::freeLoadedTexture);
    auto [new_texture, inserted] = textures_.emplace(name, std::move(ptr));
    (void)inserted;
    return new_texture->second.get();
}

SDL_Color Renderer::getTextureColor(const std::string& name, const uint32_t x, const uint32_t y) {
    SDL_Color default_color{};

    std::string path = "assets/textures/" + name + ".png";
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> surface(IMG_Load(path.c_str()),
                                                                        SDL_DestroySurface);
    if (surface == nullptr) {
        return default_color;
    }

    const int w = surface->w;
    const int h = surface->h;
    const int px = static_cast<int>(x);
    const int py = static_cast<int>(y);

    if (px < 0 || px >= w || py < 0 || py >= h) {
        return default_color;
    }

    Uint8 r = 0, g = 0, b = 0, a = 0;
    if (!SDL_ReadSurfacePixel(surface.get(), px, py, &r, &g, &b, &a)) {
        return default_color;
    }

    return SDL_Color{r, g, b, a};
}

void Renderer::setViewportSize(const int width, const int height) {
    viewport_size_.w = width;
    viewport_size_.h = height;
}

void Renderer::beginScene() const {
    // noop
}

void Renderer::endScene() const {
    SDL_RenderPresent(renderer_.get());
}

void Renderer::renderBackground(const squirrel::Background& background) {
    SDL_SetRenderDrawColor(renderer_.get(),
                           background.sky_color.r,
                           background.sky_color.g,
                           background.sky_color.b,
                           255 /* no opacity */);
    SDL_RenderClear(renderer_.get());

    const squirrel::Texture* sky_texture = loadTexture(background.sky_texture_name);
    const squirrel::Texture* ground_texture = loadTexture(background.ground_texture_name);
    const squirrel::Texture* tree_texture = loadTexture(background.trees_texture_name);

    SDL_FRect sky_dst_rect = {
        // The sky offset is intended to give the illusion of a scrolling background. By negating
        // the offset, the tiling begins off-screen to the left. By adding to the offset to the
        // width, the tiling covers the entire viewport, while the negative offset gives the
        // scrolling illusion..
        .x = -background.sky_offset_x,
        .y = 0.0f,
        .w = static_cast<float>(viewport_size_.w) + background.sky_offset_x,
        .h = sky_texture->height,
    };
    SDL_RenderTextureTiled(renderer_.get(), sky_texture->texture, nullptr, 1.0f, &sky_dst_rect);

    const float trees_offset_x = 0;
    SDL_FRect trees_dst_rect = {
        .x = -trees_offset_x,
        .y = static_cast<float>(viewport_size_.h) - tree_texture->height * 1.1,
        .w = static_cast<float>(viewport_size_.w) + trees_offset_x,
        .h = tree_texture->height,
    };
    SDL_RenderTextureTiled(renderer_.get(), tree_texture->texture, nullptr, 1.0f, &trees_dst_rect);

    // The ground should be at the bottom of the viewport.
    SDL_FRect ground_dst_rect = {
        .x = 0.0f,
        .y = static_cast<float>(viewport_size_.h) - ground_texture->height,
        .w = static_cast<float>(viewport_size_.w),
        .h = ground_texture->height,
    };
    SDL_RenderTextureTiled(
        renderer_.get(), ground_texture->texture, nullptr, 1.0f, &ground_dst_rect);
}

void Renderer::render(const Agent& agent) {
    if (agent.getSprite() == nullptr) {
        return;
    }

    SDL_FRect dst_rect = {
        .x = agent.getPosition().x,
        .y = agent.getPosition().y,
        .w = static_cast<float>(agent.getSprite()->getFrameWidth()) * 2.0f,
        .h = static_cast<float>(agent.getSprite()->getFrameHeight()) * 2.0f,
    };

    renderSprite(*agent.getSprite(),
                 agent.getAnimationState().face,
                 agent.getAnimationState().frame_number,
                 dst_rect);
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

void Renderer::freeLoadedTexture(squirrel::Texture* texture_ptr) {
    if (texture_ptr == nullptr) {
        return;
    }

    SDL_Log("Freed texture %p", reinterpret_cast<void*>(texture_ptr->texture));

    SDL_DestroyTexture(texture_ptr->texture);
    texture_ptr->texture = nullptr;
    texture_ptr->width = 0.0f;
    texture_ptr->height = 0.0f;

    delete texture_ptr;
}
