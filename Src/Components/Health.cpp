#include "Components/Health.h"

#include <algorithm>

#include "Components/Attack.h"
#include "Components/Collider.h"
#include "Events/DamageTaken.h"
#include "Events/EventQueue.h"
#include "Singleton.h"
#include "Utils/Log.h"
#include "Utils/Profiling.h"

using namespace component;

Health::Health(uint64_t max_hit_points, OnDeathCallback on_death_callback)
    : max_hit_points_(max_hit_points), hit_points_(max_hit_points), on_death_callback_(on_death_callback)
{
}

void Health::heal(uint64_t hit_points)
{
    hit_points_ += hit_points;
    hit_points_ = std::min(hit_points_, max_hit_points_);
}

void Health::heal()
{
    heal(max_hit_points_);
}

void Health::initialize()
{
    ProfileScope;

    std::shared_ptr<Collider> collider;
    GET_COMPONENT(Collider, collider, Health);

    std::weak_ptr<Health> self = std::dynamic_pointer_cast<Health>(Component::shared_from_this());
    collider->addCollisionCallback([self](GameObjectId game_object_id) {
        auto game_object_option = Singleton::scene_root.lock()->getGameObject(game_object_id);
        if (!game_object_option.has_value())
            return false;

        auto game_object = game_object_option.value();

        auto attack_option = game_object->getComponent<Attack>();
        if (!attack_option.has_value())
            return false;

        auto attack = attack_option.value();
        attack->dealDamages(self.lock());

        return false;
    });
}

void Health::damage(uint64_t hit_points)
{
    LOG_DEBUG("taken {} damages", hit_points);
 
    hit_points = std::min(hit_points, hit_points_);
    hit_points_ -= hit_points;

    if (hit_points_ == 0)
        on_death_callback_(owner_.lock());

    EventQueue::post<event::DamageTaken>(getOwner()->getId());
}

float Health::getRemainingHealthRatio() const
{
    return static_cast<float>(hit_points_) / static_cast<float>(max_hit_points_);
}

bool Health::isAlive() const
{
    return hit_points_ > 0;
}
