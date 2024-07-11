#include "PhysicsSystem.hpp"

#include <Components/RigidBodyComponent.hpp>
#include <Components/SphereColliderComponent.hpp>
#include <Components/TransformComponent.hpp>

#include <Events/ApplyForceEvent.hpp>
#include <Events/ApplyTorqueEvent.hpp>
#include <Events/CollisionEvent.hpp>

void PhysicsSystem::Start()
{
    mManager->AddEventListener<CollisionEvent>(
        [&](const CollisionEvent collisionEvent) {
            const auto& targetTransform =
                mManager->GetComponent<TransformComponent>(
                    collisionEvent.target);
            const auto& targetCollider =
                mManager->GetComponent<SphereColliderComponent>(
                    collisionEvent.target);

            const auto collisorTransform =
                mManager->GetComponent<TransformComponent>(
                    collisionEvent.collisor);
            const auto& collisorCollider =
                mManager->GetComponent<SphereColliderComponent>(
                    collisionEvent.collisor);

            const auto distance = glm::distance(
                targetTransform.position, collisorTransform.position);

            const auto force =
                (targetCollider.radius + collisorCollider.radius - distance) *
                200.0f;

            mManager->SendEvent(ApplyForceEvent {
                .target    = collisionEvent.target,
                .direction = glm::normalize(
                    targetTransform.position - collisorTransform.position),
                .magnetiude = force,
                .deltaTime  = collisionEvent.deltaTime });

            mManager->SendEvent(ApplyForceEvent {
                .target    = collisionEvent.collisor,
                .direction = glm::normalize(
                    collisorTransform.position - targetTransform.position),
                .magnetiude = force,
                .deltaTime  = collisionEvent.deltaTime });
        });

    mManager->AddEventListener<ApplyForceEvent>(
        [&](const ApplyForceEvent& applyForceEvent) {
            if (mManager->HasComponent<RigidBodyComponent>(
                    applyForceEvent.target))
            {
                auto& rigidBody = mManager->GetComponent<RigidBodyComponent>(
                    applyForceEvent.target);

                const auto acceleration = applyForceEvent.magnetiude / rigidBody.mass;

                rigidBody.velocity += applyForceEvent.direction * acceleration *
                                      applyForceEvent.deltaTime;
            }
        });

    mManager->AddEventListener<ApplyTorqueEvent>(
        [&](const ApplyTorqueEvent& applyTorqueEvent) {
            if (mManager->HasComponent<RigidBodyComponent>(
                    applyTorqueEvent.target))
            {
                auto& transform = mManager->GetComponent<TransformComponent>(
                    applyTorqueEvent.target);
                const auto& rigidBody =
                    mManager->GetComponent<RigidBodyComponent>(
                        applyTorqueEvent.target);

                const auto angularAcceleration =
                    applyTorqueEvent.magnetiude / rigidBody.mass * applyTorqueEvent.deltaTime;

                transform.rotation *= glm::angleAxis(
                    angularAcceleration,
                    applyTorqueEvent.axis);
            }
        });
}

void PhysicsSystem::Update(float deltaTime)
{
    mManager->ForEachAsync<TransformComponent, RigidBodyComponent>(
        [=](fr::Entity entity, TransformComponent& transform,
            RigidBodyComponent& rigidBody) {
            transform.position += rigidBody.velocity * deltaTime;
            rigidBody.velocity *= 1.0f - 0.8f * deltaTime;
        });
}