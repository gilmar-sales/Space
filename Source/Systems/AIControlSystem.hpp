#pragma once

#include <Freyr/Freyr.hpp>

#include "OctreeSystem.hpp"

#include "Components/AIControlledComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"
#include "Components/SquadComponent.hpp"
#include "Components/TransformComponent.hpp"

class AIControlSystem final : public fr::System
{
  public:
    explicit AIControlSystem(const Ref<fr::Scene>& scene, const Ref<OctreeSystem>& octreeSystem);

    void Update(float deltaTime) override;

  private:
    void Patrol(fr::Entity entity, AIControlledComponent& aiControlled, SquadComponent& squad,
                TransformComponent& transform);

    void Chase(AIControlledComponent& aiControlled, SpaceShipControlComponent& spaceShipControl,
               const SquadComponent& squad, TransformComponent& transform, LaserGunComponent& laserGun) const;

    void Flee(AIControlledComponent& aiControlled, SpaceShipControlComponent& spaceShipControl, float deltaTime);

    Ref<OctreeSystem> mOctree;
};