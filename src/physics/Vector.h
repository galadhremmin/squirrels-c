#pragma once

namespace squirrel {

struct Vector2f {
    float x{};
    float y{};

    constexpr Vector2f operator+(Vector2f o) const noexcept { return {x + o.x, y + o.y}; }
    constexpr Vector2f operator-(Vector2f o) const noexcept { return {x - o.x, y - o.y}; }
    constexpr Vector2f operator*(float s) const noexcept { return {x * s, y * s}; }
    constexpr Vector2f& operator+=(Vector2f o) noexcept { x += o.x; y += o.y; return *this; }
    constexpr Vector2f& operator-=(Vector2f o) noexcept { x -= o.x; y -= o.y; return *this; }
    constexpr Vector2f& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
};

} // namespace squirrel
