#include "EnemyControlSystem.hpp"

#include "Components/EnemyComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/SpaceShipControlComponent.hpp"
#include "Components/TransformComponent.hpp"

EnemyControlSystem::EnemyControlSystem(const Ref<fr::Scene>& scene) : System(scene)
{
    mPlayer = mScene->FindUnique<PlayerComponent>();
}

void EnemyControlSystem::Update(float deltaTime)
{
    mScene->TryGetComponents<TransformComponent>(mPlayer, [this, deltaTime](TransformComponent& playerTransform) {
        mScene->ForEachAsync<TransformComponent, EnemyComponent, LaserGunComponent, SpaceShipControlComponent>(
            [this, deltaTime, playerPosition = playerTransform.position](
                auto entity, TransformComponent& transform, EnemyComponent&, LaserGunComponent& laserGun,
                SpaceShipControlComponent& spaceShipControl) {
                const auto distanceVector = playerPosition - transform.position;

                const auto toTarget = glm::normalize(distanceVector);

                const auto dotProduct = glm::dot(transform.GetForwardDirection(), toTarget);

                const auto angleThreshold = glm::cos(glm::radians(90 * 0.5f));

                auto length = glm::length(distanceVector);

                auto isNearby = length < 150;

                laserGun.triggered = isNearby && dotProduct > angleThreshold;

                spaceShipControl.boost = !isNearby ? Boost : 0.0f;

                const glm::vec3 torque = glm::normalize(glm::cross(transform.GetForwardDirection(), toTarget));

                transform.Rotate(-torque, 30.0f, deltaTime);
            });
    });
}