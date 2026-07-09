#include "Renderer.h"
#include "Texture.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <format>
#include <stdexcept>

Renderer::Renderer(SDL_Renderer* renderer)
    : renderer_(renderer), font_ptr_(nullptr, TTF_CloseFont) {
    if (renderer == nullptr) {
        throw std::runtime_error("Renderer must not be null");
    }

    const int enable_vsync = 1;
    if (!SDL_SetRenderVSync(renderer_, enable_vsync)) {
        SDL_Log("VSync set to %d failed: %s", enable_vsync, SDL_GetError());
    }

    if (!TTF_Init()) {
        SDL_Log("Failed to initiate SDL3_ttf: %s", SDL_GetError());
    }

    auto font = TTF_OpenFont("assets/fonts/main.ttf", 40);
    if (font == nullptr) {
        SDL_Log("Failed to load main.ttf: %s", SDL_GetError());
    }

    font_ptr_.reset(font);
}

Renderer::~Renderer() {
    // This must be called before TTF_Quit to avoid a segfault.
    font_ptr_.reset(nullptr);
    TTF_Quit();
}

void Renderer::setViewportSize(const int width, const int height) {
    viewport_size_.w = width;
    viewport_size_.h = height;
}

void Renderer::beginScene() const {
    // noop
}

void Renderer::endScene() const {
    SDL_RenderPresent(renderer_);
}

void Renderer::renderBackground(const squirrel::Background& background) const {
    SDL_SetRenderDrawColor(renderer_,
                           background.sky_color.r,
                           background.sky_color.g,
                           background.sky_color.b,
                           255 /* no opacity */);
    SDL_RenderClear(renderer_);

    const squirrel::Texture* sky_texture = getTextureChecked(background.sky_texture_name);
    const squirrel::Texture* ground_texture = getTextureChecked(background.ground_texture_name);
    const squirrel::Texture* tree_texture = getTextureChecked(background.trees_texture_name);

    const float vw = static_cast<float>(viewport_size_.w);
    const float vh = static_cast<float>(viewport_size_.h);
    const float sky_offset_px = background.sky_offset_x * vw;

    SDL_FRect sky_dst_rect = {
        // The sky offset is intended to give the illusion of a scrolling background. By negating
        // the offset, the tiling begins off-screen to the left. By adding to the offset to the
        // width, the tiling covers the entire viewport, while the negative offset gives the
        // scrolling illusion.
        .x = -sky_offset_px,
        .y = 0.0f,
        .w = vw + sky_offset_px,
        .h = sky_texture->height,
    };
    SDL_RenderTextureTiled(renderer_, sky_texture->texture, nullptr, 1.0f, &sky_dst_rect);

    const float trees_offset_x = 0;
    SDL_FRect trees_dst_rect = {
        .x = -trees_offset_x,
        .y = vh - tree_texture->height * 1.1f,
        .w = vw + trees_offset_x,
        .h = tree_texture->height,
    };
    SDL_RenderTextureTiled(renderer_, tree_texture->texture, nullptr, 1.0f, &trees_dst_rect);

    // The ground should be at the bottom of the viewport.
    SDL_FRect ground_dst_rect = {
        .x = 0.0f,
        .y = vh - ground_texture->height,
        .w = vw,
        .h = ground_texture->height,
    };
    SDL_RenderTextureTiled(renderer_, ground_texture->texture, nullptr, 1.0f, &ground_dst_rect);
}

void Renderer::renderScore(const size_t score) const {
    const squirrel::Texture* texture;
    if (last_texture_score_ == std::numeric_limits<decltype(last_texture_score_)>::max() ||
        last_texture_score_ != score) {
        SDL_Color color = {};
        SDL_Surface* score_surface =
            TTF_RenderText_Blended(font_ptr_.get(), std::format("{}", score).c_str(), 0, color);

        if (score_surface == nullptr) {
            SDL_Log("Failed to create the score surface");
            return;
        }

        auto score_texture = SDL_CreateTextureFromSurface(renderer_, score_surface);

        SDL_DestroySurface(score_surface);

        if (score_texture == nullptr) {
            SDL_Log("Failed to create a texture from the player's score.");
            return;
        }

        texture = storeTexture("score", score_texture);
        last_texture_score_ = score;
    } else {
        texture = getTextureUnchecked("score");
    }

    auto rct = SDL_FRect{.x = 10, .y = 10, .w = texture->width, .h = texture->height};
    SDL_RenderTexture(renderer_, texture->texture, nullptr, &rct);
}

void Renderer::render(const Agent& agent) const {
    if (agent.getSprite() == nullptr) {
        return;
    }

    const float vw = static_cast<float>(viewport_size_.w);
    const float vh = static_cast<float>(viewport_size_.h);

    SDL_FRect dst_rect = {
        .x = agent.getPosition().x * vw,
        .y = agent.getPosition().y * vh,
        .w = agent.getSize().w * vw,
        .h = agent.getSize().h * vh,
    };

    renderSprite(*agent.getSprite(),
                 agent.getAnimationState().face,
                 agent.getAnimationState().frame_number,
                 dst_rect);

    if (render_boundary_boxes_) {
        SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 0);
        SDL_RenderRect(renderer_, &dst_rect);
    }
}

void Renderer::renderSprite(const Sprite& sprite,
                            const SpriteAnimationFace face,
                            const uint8_t frame_number,
                            const SDL_FRect& dst_rect) const {
    SDL_FRect src_rect = {
        .x = static_cast<float>(frame_number * sprite.getFrameWidth()),
        .y = static_cast<float>(sprite.getOffsetIndex(face) * sprite.getFrameHeight()),
        .w = static_cast<float>(sprite.getFrameWidth()),
        .h = static_cast<float>(sprite.getFrameHeight()),
    };

    SDL_RenderTexture(renderer_, sprite.getTexturePtr(), &src_rect, &dst_rect);
}

const squirrel::Texture* Renderer::loadTexture(const std::string& name) {
    if (auto texture = getTextureUnchecked(name)) {
        return texture;
    }

    auto path = std::format("assets/textures/{}.png", name);

    SDL_Texture* texture = IMG_LoadTexture(renderer_, path.c_str());
    if (texture == nullptr) {
        SDL_Log("Failed to load texture: %s", path.c_str());
        throw std::runtime_error("Failed to load texture");
    }

    return storeTexture(name, texture);
}

const squirrel::Texture* Renderer::getTextureChecked(const std::string& name) const {
    auto texture = getTextureUnchecked(name);
    if (texture == nullptr) {
        throw std::runtime_error(std::format("[Renderer] {} isn't a loaded texture.", name));
    }

    return texture;
}

const squirrel::Texture* Renderer::storeTexture(const std::string& name,
                                                SDL_Texture* texture) const {
    float tex_w = 0, tex_h = 0;
    if (!SDL_GetTextureSize(texture, &tex_w, &tex_h)) {
        SDL_Log("Failed to query texture: %s", name.c_str());
        throw std::runtime_error(std::format("Failed to query texture {}", name));
    }

    SDL_Log("Loaded texture %s (%p)", name.c_str(), reinterpret_cast<void*>(texture));

    auto ptr = std::unique_ptr<squirrel::Texture, decltype(&Renderer::freeTexture)>(
        new squirrel::Texture{
            .texture = texture,
            .width = tex_w,
            .height = tex_h,
        },
        &Renderer::freeTexture);
    auto [new_texture, inserted] = textures_.insert_or_assign(name, std::move(ptr));
    (void)inserted;

    return new_texture->second.get();
}

const squirrel::Texture* Renderer::getTextureUnchecked(const std::string& name) const {
    // I'm not using contains here because I want to avoid an extra lookup for textures already
    // loaded.
    auto existing_texture = textures_.find(name);
    if (existing_texture != textures_.end()) {
        return existing_texture->second.get();
    }

    return nullptr;
}

SDL_Color Renderer::getTextureColor(const std::string& name, const uint32_t x, const uint32_t y) {
    SDL_Color default_color{};

    auto path = std::format("assets/textures/{}.png", name);
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

void Renderer::freeTexture(squirrel::Texture* texture_ptr) {
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
