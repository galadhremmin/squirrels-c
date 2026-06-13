#pragma once

#include <cstdint>

struct ViewportBounds {
    float left;   // always 0.0f (normalized)
    float top;    // always 0.0f (normalized)
    float right;  // always 1.0f (normalized)
    float bottom; // logical ground in normalized space (e.g. 0.85f)
};

enum class ViewportEdge : uint8_t {
    None    = 0,
    Left    = 1 << 0,
    Right   = 1 << 1,
    Top     = 1 << 2,
    Bottom  = 1 << 3,
    Outside = 1 << 4,
};

constexpr ViewportEdge operator|(ViewportEdge a, ViewportEdge b) noexcept {
    return static_cast<ViewportEdge>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
constexpr ViewportEdge& operator|=(ViewportEdge& a, ViewportEdge b) noexcept {
    return a = a | b;
}
constexpr bool operator&(ViewportEdge a, ViewportEdge b) noexcept {
    return static_cast<uint8_t>(a) & static_cast<uint8_t>(b);
}
