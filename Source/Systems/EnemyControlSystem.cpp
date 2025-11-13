#include "EnemyControlSystem.hpp"

#include "Components/EnemyComponent.hpp"
#include "Components/LaserGunComponent.hpp"
#include "Components/PlayerComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Events/ApplyTorqueEvent.hpp"

EnemyControlSystem::EnemyControlSystem(const Ref<fr::Scene>& scene) : System(scene)
{
    mPlayer = mScene->FindUnique<PlayerComponent>();
}

void EnemyControlSystem::Update(float deltaTime)
{
    mScene->TryGetComponents<TransformComponent>(mPlayer, [this, deltaTime](TransformComponent& playerTransform) {
        mScene->ForEachAsync<TransformComponent, EnemyComponent, LaserGunComponent>(
            [this, deltaTime, playerPosition = playerTransform.position](
                auto entity, TransformComponent& transform, EnemyComponent&, LaserGunComponent& laserGun) {
                const auto distanceVector = playerPosition - transform.position;

                const auto toTarget       = glm::normalize(distanceVector);

                const auto dotProduct = glm::dot(transform.GetForwardDirection(), toTarget);

                const auto angleThreshold = glm::cos(glm::radians(90 * 0.5f));

                laserGun.triggered = glm::length(distanceVector) < 150 && dotProduct > angleThreshold;

                const glm::vec3 torque = glm::normalize(glm::cross(transform.GetForwardDirection(), toTarget));


                mScene->SendEvent(ApplyTorqueEvent {
                    .target     = entity,
                    .axis       = -torque,
                    .magnetiude = 2000.0f,
                    .deltaTime  = deltaTime });
            });
    });
}