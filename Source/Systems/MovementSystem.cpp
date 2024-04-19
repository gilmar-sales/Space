#include "MovementSystem.hpp"

#include <Components/TransformComponent.hpp>
#include <Components/RigidBodyComponent.hpp>

#include <Events/CollisionEvent.hpp>

#include <random>
#include <glm/ext/matrix_transform.hpp>
#include <Events/ApplyForceEvent.hpp>

std::uint32_t randomN(std::uint32_t min, std::uint32_t max)
{
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<> uniform_dist(min, max);

    return uniform_dist(e1);
}

void MovementSystem::Update(float deltaTime)
{
    mManager->ForEachAsync<TransformComponent, RigidBodyComponent>([manager = mManager, deltaTime = deltaTime]
    (fr::Entity entity, TransformComponent &transform, RigidBodyComponent &rigidBody)
    {
        manager->SendEvent(ApplyForceEvent{
            .target = entity,
            .direction = glm::vec3(0.0, 0.0, 1.0),
            .magnetiude = float(200),
            .deltaTime = deltaTime
        });
    });
}
