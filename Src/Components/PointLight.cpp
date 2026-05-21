#include "Components/PointLight.h"

#include "Utils/Log.h"
#include "Utils/Profiling.h"

component::PointLight::PointLight(Color c, float i) : color(c), intensity(i)
{
}

component::PointLight::~PointLight()
{
}

void component::PointLight::initialize(std::vector<std::shared_ptr<resource::Shader>> shaders)
{
    for (auto shader : shaders)
    {
        shaders_.push_back(shader);
    }
}

void component::PointLight::beginPreRender()
{
    lights_drawn_on_this_frame_ = 0;
}

void component::PointLight::endPreRender()
{
    for (auto weak_shader : shaders_)
    {
        auto shader = weak_shader.lock();

        shader->bind();
        shader->setUniform("u_PointLightCount", static_cast<int32_t>(lights_drawn_on_this_frame_));
    }
}

void component::PointLight::preRender(glm::mat4 &transform, RenderPass pass) const
{
    ProfileScope;
    ProfileScopeGPU("PointLight::preRender");

    if (pass == RenderPass::Shadow)
    {
        return;
    }

    (void)pass;

    if (lights_drawn_on_this_frame_ >= MAX_POINT_LIGHTS)
    {
        LOG_WARNING("Too many point lights to draw, skipping");
        return;
    }

    const auto position = glm::vec3(transform[3]);

    for (auto weak_shader : shaders_)
    {
        auto shader = weak_shader.lock();

        shader->bind();
        shader->setUniformArrayElement("u_PointLights[{}].position", lights_drawn_on_this_frame_, position);
        shader->setUniformArrayElement("u_PointLights[{}].color", lights_drawn_on_this_frame_, glm::vec3(color));
        shader->setUniformArrayElement("u_PointLights[{}].intensity", lights_drawn_on_this_frame_, intensity);
    }

    lights_drawn_on_this_frame_ += 1;
}
