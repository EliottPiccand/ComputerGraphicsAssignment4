#pragma once

#include <memory>

#include <Lib/OpenGL.h>
#include <Lib/glm.h>

#include "Components/Component.h"
#include "Resources/Shader.h"
#include "Utils/Color.h"

namespace component
{

/// Only one DirectionalLight should be drawn each frame
class DirectionalLight : public Component
{
  public:
    static inline constexpr const float AMBIENT_INTENSITY = 0.3f;
    static inline constexpr const Color AMBIENT_COLOR = color::SKY;

    /// direction must be normalized
    DirectionalLight(glm::vec3 direction, Color color, float intensity);
    ~DirectionalLight() override;

    static void initialize(std::vector<std::shared_ptr<resource::Shader>> shaders);

    static void beginPreRender();
    void preRender(glm::mat4 &transform) const override;

    /// normalized
    glm::vec3 direction;
    Color color;
    float intensity;

  private:
    static inline std::vector<std::weak_ptr<resource::Shader>> shaders_;
};

} // namespace component
