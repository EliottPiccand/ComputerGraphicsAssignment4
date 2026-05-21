#pragma once

#include <Lib/OpenGL.h>
#include <Lib/glm.h>

using Color = glm::vec4;

#define rgba(r, g, b, a)                                                                                               \
    Color                                                                                                              \
    {                                                                                                                  \
        r##.0f / 255.0f, g##.0f / 255.0f, b##.0f / 255.0f, static_cast<float>(a)                                       \
    }
#define rgb(r, g, b) rgba(r, g, b, 1)

namespace color
{

// clang-format off
constexpr const Color WHITE       = rgb(255, 255, 255);
constexpr const Color BLACK       = rgb(0, 0, 0);
constexpr const Color TRANSPARENT = rgba(0, 0, 0, 0);
constexpr const Color RED         = rgb(255, 0, 0);
constexpr const Color GREEN       = rgb(0, 255, 0);
constexpr const Color BLUE        = rgb(0, 0, 255);
constexpr const Color YELLOW      = rgb(255, 255, 0);

constexpr const Color SKY = rgb(193, 234, 255);
constexpr const Color SUN = rgb(252, 231, 165);
// clang-format on

inline Color hsv(float h, float s, float v) {
    float h_normalized = std::fmod(h / 60.0f, 6.0f);
    if (h_normalized < 0.0f) {
        h_normalized += 6.0f;
    }

    float f = h_normalized - static_cast<float>(static_cast<int32_t>(h_normalized));
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    float r, g, b;
    
    int32_t hi = static_cast<int32_t>(h_normalized);

    switch (hi) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default: r = g = b = 0.0f; break; // Should not happen with valid input
    }

    return Color{r, g, b, 1.0f};
}

} // namespace color
