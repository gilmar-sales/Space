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
    explicit AIControlSystem(const skr::Arc<fr::Registry>& registry, const skr::Arc<OctreeSystem>& octreeSystem);

    void Update(float deltaTime) override;

  private:
    void Patrol(fr::Entity entity, AIControlledComponent& aiControlled, SquadComponent& squad,
                TransformComponent& transform, SpaceShipControlComponent& spaceShipControl);

    void Chase(fr::Entity entity, AIControlledComponent& aiControlled, SpaceShipControlComponent& spaceShipControl,
               const SquadComponent& squad, TransformComponent& transform, LaserGunComponent& laserGun,
               float deltaTime);

    void Flee(fr::Entity entity, AIControlledComponent& aiControlled, SpaceShipControlComponent& spaceShipControl,
              TransformComponent& transform, float deltaTime);

    [[nodiscard]] std::optional<fr::Entity> FindBestTarget(fr::Entity entity, const AIControlledComponent& ai,
                                                           const SquadComponent& squad,
                                                           const TransformComponent& transform,
                                                           fr::Entity exclude = 0) const;

    [[nodiscard]] std::optional<fr::Entity> FindSquadLeaderTarget(fr::Entity entity, AIControlledComponent& ai,
                                                                  const SquadComponent& squad,
                                                                  const TransformComponent& transform) const;

    [[nodiscard]] int CountPursuers(fr::Entity target, const SquadComponent& squad, fr::Entity self,
                                    const glm::vec3& targetPosition) const;

    [[nodiscard]] glm::vec3 ComputeSeparation(fr::Entity entity, const SquadComponent& squad,
                                              const TransformComponent& transform) const;

    [[nodiscard]] glm::vec3 ComputeObstacleAvoidance(fr::Entity entity, const TransformComponent& transform) const;

    [[nodiscard]] glm::vec3 ComputeCombatAim(fr::Entity entity, AIControlledComponent& ai,
                                             const TransformComponent& transform,
                                             const TransformComponent& targetTransform, const glm::vec3& toTarget,
                                             float distance, float shootRange, float deltaTime) const;

    static void ApplySteering(SpaceShipControlComponent& spaceShipControl, const TransformComponent& transform,
                              const glm::vec3& desiredDirection, const glm::vec3& faceDirection);

    static void UpdateBoost(AIControlledComponent& aiControlled, SpaceShipControlComponent& spaceShipControl,
                            float alignment, float distance, float deltaTime);

    skr::Arc<OctreeSystem> mOctree;
};
