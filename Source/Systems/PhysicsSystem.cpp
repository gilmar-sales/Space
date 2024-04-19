#include "PhysicsSystem.hpp"

#include <glm/ext/matrix_transform.hpp>

#include <Components/RigidBodyComponent.hpp>
#include <Components/SphereColliderComponent.hpp>
#include <Components/TransformComponent.hpp>

#include <Events/ApplyForceEvent.hpp>
#include <Events/CollisionEvent.hpp>

void PhysicsSystem::Start()
{
    mManager->AddEventListener<CollisionEvent>([&](CollisionEvent collisionEvent) {
        auto& targetTransform = mManager->GetComponent<TransformComponent>(collisionEvent.target);
        auto& targetCollider  = mManager->GetComponent<SphereColliderComponent>(collisionEvent.target);

        auto  collisorTransform = mManager->GetComponent<TransformComponent>(collisionEvent.collisor);
        auto& collisorCollider  = mManager->GetComponent<SphereColliderComponent>(collisionEvent.collisor);

        auto distance = glm::distance(targetTransform.position, collisorTransform.position);

        auto force = (targetCollider.radius + collisorCollider.radius - distance) * 200.0f;

        mManager->SendEvent(ApplyForceEvent {
            .target     = collisionEvent.target,
            .direction  = glm::normalize(targetTransform.position - collisorTransform.position),
            .magnetiude = force,
            .deltaTime  = collisionEvent.deltaTime });

        mManager->SendEvent(ApplyForceEvent {
            .target     = collisionEvent.collisor,
            .direction  = glm::normalize(collisorTransform.position - targetTransform.position),
            .magnetiude = force,
            .deltaTime  = collisionEvent.deltaTime });
    });

    mManager->AddEventListener<ApplyForceEvent>([&](ApplyForceEvent applyForceEvent) {
        if (mManager->HasComponent<RigidBodyComponent>(applyForceEvent.target))
        {
            auto& transform = mManager->GetComponent<TransformComponent>(applyForceEvent.target);
            auto& rigidBody = mManager->GetComponent<RigidBodyComponent>(applyForceEvent.target);

            auto acceleration = applyForceEvent.magnetiude / rigidBody.mass;

            rigidBody.velocity += applyForceEvent.direction * acceleration * applyForceEvent.deltaTime;
        }
    });
}

void PhysicsSystem::Update(float deltaTime)
{
    mManager->ForEachAsync<TransformComponent, RigidBodyComponent>([=](fr::Entity entity, TransformComponent& transform, RigidBodyComponent& rigidBody) {
        transform.position += rigidBody.velocity * deltaTime;
        rigidBody.velocity *= 1.0f - 0.07f * deltaTime;
    });
}