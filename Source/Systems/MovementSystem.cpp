#include "MovementSystem.hpp"

#include <glm/ext/matrix_transform.hpp>

#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"

#include "Events/ApplyForceEvent.hpp"
#include "Events/CollisionEvent.hpp"

void MovementSystem::Update(float deltaTime)
{
    mManager->ForEachAsync<TransformComponent, RigidBodyComponent>(
        [manager = mManager, deltaTime = deltaTime](
            fr::Entity entity, TransformComponent& transform,
            RigidBodyComponent& rigidBody) {
            manager->SendEvent(ApplyForceEvent {
                .target     = entity,
                .direction  = glm::vec3(0.0, 0.0, 1.0),
                .magnetiude = float(200),
                .deltaTime  = deltaTime });
        });
}
