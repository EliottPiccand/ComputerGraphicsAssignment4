#pragma once

#include <memory>
#include <tuple>

#include <Lib/OpenGL.h>
#include <Lib/glm.h>
#include <vector>

#include "Components/Component.h"
#include "Resources/Shader.h"
#include "Resources/Texture.h"

namespace component
{

/// Must be attached to the scene root only
class Sky : public Component
{
  public:
    struct SkyboxEntry
    {
        std::weak_ptr<resource::Texture> texture;
        float start_time;
        float end_time;
    };

    Sky(std::vector<SkyboxEntry> textures, std::vector<std::weak_ptr<resource::Shader>> shaders);

    void render(glm::mat4 &transform) const override;

  private:
    std::vector<SkyboxEntry> textures_;
    std::vector<std::weak_ptr<resource::Shader>> shaders_;
};

} // namespace component
