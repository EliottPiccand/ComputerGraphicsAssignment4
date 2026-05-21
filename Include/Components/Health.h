#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "Components/Component.h"
#include "GameObject.h"

namespace component
{

class Health : public Component
{
  public:
    using OnDeathCallback = std::function<void(std::shared_ptr<GameObject> game_object)>;

    Health(uint64_t max_hit_points, OnDeathCallback on_death_callback);

    void initialize() override;

    void damage(uint64_t hit_points);
    void heal(uint64_t hit_points);
    void heal();

    float getRemainingHealthRatio() const;
    bool isAlive() const;

  private:
    const uint64_t max_hit_points_;
    uint64_t hit_points_;

    OnDeathCallback on_death_callback_;
};

} // namespace component
