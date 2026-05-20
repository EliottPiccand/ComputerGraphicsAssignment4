#pragma once

#include <limits>
#include <memory>

#include <Lib/OpenGL.h>
#include <Lib/glm.h>

#include "Components/Component.h"
#include "Resources/Shader.h"
#include "Utils/Color.h"


namespace component
{

class PointLight : public Component
{
  public:
    static inline constexpr const size_t MAX_POINT_LIGHTS = 16;
    static_assert(MAX_POINT_LIGHTS <= std::numeric_limits<int32_t>::max(), "MAX_POINT_LIGHTS must fit in an int32_t");

    /// direction must be normalized
    PointLight(Color color, float intensity);
    ~PointLight() override;

    static void initialize(std::vector<std::shared_ptr<resource::Shader>> shaders);

    static void beginPreRender();
    static void endPreRender();
    void preRender(glm::mat4 &transform) const override;

    Color color;
    float intensity;

  private:
    static inline std::vector<std::weak_ptr<resource::Shader>> shaders_;
    static inline size_t lights_drawn_on_this_frame_;
};

} // namespace component
