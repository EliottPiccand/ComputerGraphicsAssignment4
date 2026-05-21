#include "Components/Component.h"

using namespace component;

void Component::initialize()
{
}

void Component::update()
{
}

void Component::preRender(glm::mat4 &transform, RenderPass pass) const
{
    (void)transform;
    (void)pass;
}

void Component::render(glm::mat4 &transform, RenderPass pass) const
{
    (void)transform;
    (void)pass;
}

void Component::renderDefered(glm::mat4 &transform, RenderPass pass) const
{
    (void)transform;
    (void)pass;
}

void Component::setOwner(std::shared_ptr<GameObject> game_object)
{
    owner_ = game_object;
}

std::shared_ptr<GameObject> Component::getOwner() const
{
    return owner_.lock();
}
