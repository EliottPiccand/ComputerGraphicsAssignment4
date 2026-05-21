#include "Components/DirectionalLight.h"

#include <array>

#include "Resources/ResourceLoader.h"
#include "Resources/Texture.h"
#include "Utils/Constants.h"
#include "Utils/Profiling.h"

namespace
{
glm::vec3 chooseUpVector(const glm::vec3 &direction)
{
    const float alignment = glm::abs(glm::dot(glm::normalize(direction), UP));
    return alignment > 0.95f ? EAST : UP;
}
} // namespace

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

void component::DirectionalLight::initializeShadow()
{
    if (shadow_frame_buffer_ != 0)
    {
        return;
    }

    shadow_shaders_.push_back(ResourceLoader::get<resource::Shader>("Shadow"));
    shadow_shaders_.push_back(ResourceLoader::get<resource::Shader>("Shadow#FLAP"));

    glGenFramebuffers(1, &shadow_frame_buffer_);
    glGenTextures(1, &shadow_depth_texture_);

    glBindTexture(GL_TEXTURE_2D, shadow_depth_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    const GLfloat border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glBindFramebuffer(GL_FRAMEBUFFER, shadow_frame_buffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_depth_texture_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void component::DirectionalLight::beginPreRender()
{
    for (auto weak_shader : shaders_)
    {
        auto shader = weak_shader.lock();

        shader->bind();
        shader->setUniform("u_AmbientLight", glm::vec3(AMBIENT_COLOR * AMBIENT_INTENSITY));
        shader->setUniform("u_LightSpaceMatrix", light_space_matrix_);
        glBindTextureUnit(resource::Texture::SHADOW_MAP_SLOT, shadow_depth_texture_);
        shader->setUniform("u_ShadowMap", static_cast<int32_t>(resource::Texture::SHADOW_MAP_SLOT));
    }
}

void component::DirectionalLight::beginShadowRender(glm::vec3 focus_point)
{
    ProfileScope;
    ProfileScopeGPU("DirectionalLight::beginShadowRender");

    current_focus_point_ = focus_point;
    light_space_matrix_ = calculateLightSpaceMatrix(current_direction_, current_focus_point_);

    for (auto weak_shader : shadow_shaders_)
    {
        auto shader = weak_shader.lock();
        shader->bind();
        shader->setUniform("u_LightSpaceMatrix", light_space_matrix_);
    }

    glGetIntegerv(GL_VIEWPORT, previous_viewport_.data());

    glBindFramebuffer(GL_FRAMEBUFFER, shadow_frame_buffer_);
    glViewport(0, 0, static_cast<GLsizei>(SHADOW_MAP_SIZE), static_cast<GLsizei>(SHADOW_MAP_SIZE));
    glClear(GL_DEPTH_BUFFER_BIT);
}

void component::DirectionalLight::endShadowRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(previous_viewport_[0], previous_viewport_[1], previous_viewport_[2], previous_viewport_[3]);
}

void component::DirectionalLight::preRender(glm::mat4 &transform, RenderPass pass) const
{
    ProfileScope;
    ProfileScopeGPU("DirectionalLight::preRender");

    (void)transform;

    if (pass == RenderPass::Shadow)
    {
        current_direction_ = direction;
        return;
    }

    for (auto weak_shader : shaders_)
    {
        auto shader = weak_shader.lock();

        shader->bind();
        shader->setUniform("u_DirectionalLightDirection", direction);
        shader->setUniform("u_DirectionalLightColor", intensity * glm::vec3(color));
    }
}

glm::mat4 component::DirectionalLight::calculateLightSpaceMatrix(glm::vec3 light_direction, glm::vec3 focus_point)
{
    const glm::vec3 light_position = focus_point - glm::normalize(light_direction) * WORLD_WIDTH;
    const glm::mat4 view = glm::lookAt(light_position, focus_point, chooseUpVector(light_direction));
    const float extent = WORLD_WIDTH * 0.9f;
    const glm::mat4 projection = glm::ortho(-extent, extent, -extent, extent, -extent, extent * 2.0f);
    return projection * view;
}
