#pragma once

#include <memory>
#include <array>
#include <vector>

#include <Lib/OpenGL.h>
#include <Lib/glm.h>

#include "Components/Component.h"
#include "Resources/Shader.h"
#include "Utils/Color.h"
#include "Utils/Constants.h"
#include "Utils/Math.h"

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
    static void initializeShadow();

    static void beginPreRender();
    static void beginShadowRender(glm::vec3 focus_point);
    static void endShadowRender();

    void preRender(glm::mat4 &transform, RenderPass pass = RenderPass::Main) const override;

    /// normalized
    glm::vec3 direction;
    Color color;
    float intensity;

  private:
    static glm::mat4 calculateLightSpaceMatrix(glm::vec3 light_direction, glm::vec3 focus_point);

    static inline constexpr const GLuint SHADOW_MAP_SIZE = 4096;
    static inline std::vector<std::weak_ptr<resource::Shader>> shaders_;
    static inline std::vector<std::weak_ptr<resource::Shader>> shadow_shaders_;
    static inline GLuint shadow_frame_buffer_ = 0;
    static inline GLuint shadow_depth_texture_ = 0;
    static inline glm::vec3 current_direction_ = -UP;
    static inline glm::vec3 current_focus_point_ = ZERO;
    static inline glm::mat4 light_space_matrix_ = glm::mat4(1.0f);
    static inline std::array<GLint, 4> previous_viewport_ = {0, 0, 0, 0};
};

} // namespace component
