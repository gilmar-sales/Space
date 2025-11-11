#include "EnemyControlSystem.hpp"

#include "Components/EnemyComponent.hpp"
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
        mScene->ForEachAsync<TransformComponent, EnemyComponent>(
            [this, deltaTime,
             playerPosition = playerTransform.position](auto entity, TransformComponent& transform, EnemyComponent&) {
                const glm::vec3 toTarget = glm::normalize(playerPosition - transform.position);
                const glm::vec3 torque   = glm::cross(transform.GetForwardDirection(), toTarget);

                mScene->SendEvent(
                    ApplyTorqueEvent { .target = entity, .axis = -torque, .magnetiude = 6000.0f, .deltaTime = deltaTime });
            });
    });
}