#include "Components/Sky.h"

#include <Lib/OpenGL.h>
#include <Lib/glm.h>
#include <Lib/stb.h>

#include "Utils/Time.h"
#include "Resources/Model.h"
#include "Resources/ResourceLoader.h"
#include "Resources/Shader.h"
#include "Resources/Texture.h"
#include "Utils/Profiling.h"

using namespace component;

namespace
{

struct SkyboxSample
{
    std::shared_ptr<resource::Texture> current;
    std::shared_ptr<resource::Texture> next;
    float blend;
};

SkyboxSample resolveSkybox(const std::vector<Sky::SkyboxEntry> &textures, float time_of_day)
{
    constexpr const float BLEND_FRACTION = 0.25f;
    constexpr const float MIN_BLEND_WINDOW = 0.05f;
    constexpr const float EPSILON = 1e-4f;

    if (textures.empty())
    {
        return {};
    }

    const float time = glm::fract(time_of_day);
    const size_t texture_count = textures.size();

    for (size_t i = 0; i < texture_count; ++i)
    {
        const auto &entry = textures[i];
        float interval = entry.end_time - entry.start_time;
        if (interval <= 0.0f)
        {
            interval += 1.0f;
        }

        float position = time - entry.start_time;
        if (position < 0.0f)
        {
            position += 1.0f;
        }

        if (position >= interval)
        {
            continue;
        }

        auto current = entry.texture.lock();
        if (!current)
        {
            break;
        }

        auto next = textures[(i + 1) % texture_count].texture.lock();
        if (!next)
        {
            next = current;
        }

        interval = std::max(interval, EPSILON);
        const float blend_window = glm::clamp(interval * BLEND_FRACTION, MIN_BLEND_WINDOW, interval);
        const float blend_start = interval - blend_window;

        if (position <= blend_start)
        {
            return {current, current, 0.0f};
        }

        const float blend = glm::clamp((position - blend_start) / std::max(blend_window, EPSILON), 0.0f, 1.0f);
        return {current, next, blend};
    }

    for (size_t i = 0; i < texture_count; ++i)
    {
        auto current = textures[i].texture.lock();
        if (current)
        {
            return {current, current, 0.0f};
        }
    }

    return {};
}

void bindSkyEnvironmentMaps(const std::shared_ptr<resource::Shader> &shader, const SkyboxSample &sample)
{
    shader->bind();
    shader->setUniform("u_UseAlbedoTextures", true);
    shader->setUniform("u_EnvironmentMapBlend", sample.blend);

    sample.current->bind(resource::Texture::ENVIRONMENT_MAP_SLOT_1, shader, "u_EnvironmentMap1");
    sample.next->bind(resource::Texture::ENVIRONMENT_MAP_SLOT_2, shader, "u_EnvironmentMap2");
}

void bindSceneEnvironmentMaps(const std::shared_ptr<resource::Shader> &shader, const SkyboxSample &sample)
{
    shader->bind();
    shader->setUniform("u_EnvironmentMapBlend", sample.blend);

    sample.current->bind(resource::Texture::ENVIRONMENT_MAP_SLOT_1, shader, "u_EnvironmentMap1");
    sample.next->bind(resource::Texture::ENVIRONMENT_MAP_SLOT_2, shader, "u_EnvironmentMap2");
}

} // namespace

Sky::Sky(std::vector<SkyboxEntry> textures, std::vector<std::weak_ptr<resource::Shader>> shaders)
    : textures_(std::move(textures)), shaders_(std::move(shaders))
{
}

void Sky::render(glm::mat4 &transform, RenderPass pass) const
{
    ProfileScope;
    ProfileScopeGPU("Sky::render");

    (void)transform;

    if (pass == RenderPass::Shadow)
    {
        return;
    }

    (void)pass;

    static std::weak_ptr weak_sky_shader = ResourceLoader::get<resource::Shader>("Sky");
    static std::shared_ptr<resource::Model> cube_model = ResourceLoader::get<resource::Model>("SkyCube");

    const auto sample = resolveSkybox(textures_, Time::getTimeOfDay());
    if (!sample.current)
    {
        return;
    }

    SCOPE_DEPTH_MASK(GL_FALSE);
    SCOPE_DISABLE(GL_DEPTH_TEST);
    SCOPE_DISABLE(GL_CULL_FACE);

    auto sky_shader = weak_sky_shader.lock();
    bindSkyEnvironmentMaps(sky_shader, sample);
    cube_model->draw(sky_shader);

    for (auto weak_shader : shaders_)
    {
        auto shader = weak_shader.lock();
        bindSceneEnvironmentMaps(shader, sample);
    }
}
