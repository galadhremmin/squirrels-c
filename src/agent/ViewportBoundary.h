#pragma once
#include <cstdint>

struct ViewportBounds {
    float left;   // always 0.0f (normalized)
    float top;    // always 0.0f (normalized)
    float right;  // always 1.0f (normalized)
    float bottom; // logical ground in normalized space (e.g. 0.85f)
};

enum ViewportEdge : uint8_t {
    VIEWPORT_EDGE_NONE = 0,
    VIEWPORT_EDGE_LEFT = 1 << 0,
    VIEWPORT_EDGE_RIGHT = 1 << 1,
    VIEWPORT_EDGE_TOP = 1 << 2,
    VIEWPORT_EDGE_BOTTOM = 1 << 3,
    VIEWPORT_OUTSIDE = 1 << 4,
};
