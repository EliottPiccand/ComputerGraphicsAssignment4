#include "Components/DirectionalLight.h"

#include "Utils/Profiling.h"

component::DirectionalLight::DirectionalLight(glm::vec3 dir, Color c, float i)
    : direction(dir), color(c), intensity(i)
{
}

component::DirectionalLight::~DirectionalLight()
{
}

void component::DirectionalLight::initialize(std::vector<std::shared_ptr<resource::Shader>> shaders)
{
    for (auto shader : shaders)
    {
        shaders_.push_back(shader);
    }
}

void component::DirectionalLight::beginPreRender()
{
    for (auto weak_shader : shaders_)
    {
        auto shader = weak_shader.lock();

        shader->bind();
        shader->setUniform("u_AmbientLight", glm::vec3(AMBIENT_COLOR * AMBIENT_INTENSITY));
    }
}

void component::DirectionalLight::preRender(glm::mat4 &transform) const
{
    ProfileScope;
    ProfileScopeGPU("DirectionalLight::preRender");

    (void)transform;

    for (auto weak_shader : shaders_)
    {
        auto shader = weak_shader.lock();

        shader->bind();
        shader->setUniform("u_DirectionalLightDirection", direction);
        shader->setUniform("u_DirectionalLightColor", intensity * glm::vec3(color));
    }
}
