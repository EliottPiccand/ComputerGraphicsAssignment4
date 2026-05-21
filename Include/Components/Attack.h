#pragma once

#include <memory>
#include <unordered_map>

#include "Components/Component.h"
#include "Components/Health.h"
#include "GameObject.h"
#include "Utils/Time.h"

namespace component
{

class Attack : public Component
{
  public:
    Attack(uint64_t min_damages, uint64_t max_damages, Duration min_hit_delay);

    void update() override;
    void dealDamages(std::shared_ptr<Health> to);

  private:
    const uint64_t min_damages_;
    const uint64_t max_damages_;
    const Duration min_hit_delay_;

    std::unordered_map<GameObjectId, Instant> last_hits_;
};

} // namespace component
