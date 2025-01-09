#include "PhysicsSystem.hpp"

#include "Events/TransformChangeEvent.hpp"

#include <Components/RigidBodyComponent.hpp>
#include <Components/SphereColliderComponent.hpp>
#include <Components/TransformComponent.hpp>

#include <Events/ApplyForceEvent.hpp>
#include <Events/ApplyTorqueEvent.hpp>
#include <Events/CollisionEvent.hpp>

PhysicsSystem::PhysicsSystem(const std::shared_ptr<fr::Scene>& scene) :
    System(scene)
{
    mScene->AddEventListener<CollisionEvent>(
        [&](const CollisionEvent collisionEvent) {
            const auto& targetTransform =
                mScene->GetComponent<TransformComponent>(collisionEvent.target);
            const auto& targetCollider =
                mScene->GetComponent<SphereColliderComponent>(
                    collisionEvent.target);

            const auto collisorTransform =
                mScene->GetComponent<TransformComponent>(
                    collisionEvent.collisor);
            const auto& collisorCollider =
                mScene->GetComponent<SphereColliderComponent>(
                    collisionEvent.collisor);

            const auto distance = glm::distance(
                targetTransform.position, collisorTransform.position);

            const auto force =
                (targetCollider.radius + collisorCollider.radius - distance) *
                200.0f;

            mScene->SendEvent(ApplyForceEvent {
                .target    = collisionEvent.target,
                .direction = glm::normalize(
                    targetTransform.position - collisorTransform.position),
                .magnetiude = force,
                .deltaTime  = collisionEvent.deltaTime });

            mScene->SendEvent(ApplyForceEvent {
                .target    = collisionEvent.collisor,
                .direction = glm::normalize(
                    collisorTransform.position - targetTransform.position),
                .magnetiude = force,
                .deltaTime  = collisionEvent.deltaTime });
        });

    mScene->AddEventListener<ApplyForceEvent>(
        [this](const ApplyForceEvent& applyForceEvent) {
            if (mScene->HasComponent<RigidBodyComponent>(
                    applyForceEvent.target))
            {
                auto& rigidBody = mScene->GetComponent<RigidBodyComponent>(
                    applyForceEvent.target);

                const auto acceleration =
                    applyForceEvent.magnetiude / rigidBody.mass;

                rigidBody.velocity += applyForceEvent.direction * acceleration *
                                      applyForceEvent.deltaTime;

                mScene->SendEvent(
                    TransformChangeEvent { .entity = applyForceEvent.target });
            }
        });

    mScene->AddEventListener<ApplyTorqueEvent>(
        [this](const ApplyTorqueEvent& applyTorqueEvent) {
            if (mScene->HasComponent<RigidBodyComponent>(
                    applyTorqueEvent.target))
            {
                auto& transform = mScene->GetComponent<TransformComponent>(
                    applyTorqueEvent.target);
                const auto& rigidBody =
                    mScene->GetComponent<RigidBodyComponent>(
                        applyTorqueEvent.target);

                const auto angularAcceleration =
                    applyTorqueEvent.magnetiude / rigidBody.mass *
                    applyTorqueEvent.deltaTime;

                transform.rotation *=
                    glm::angleAxis(angularAcceleration, applyTorqueEvent.axis);

                mScene->SendEvent(
                    TransformChangeEvent { .entity = applyTorqueEvent.target });
            }
        });
}
void PhysicsSystem::Update(float deltaTime)
{
    mScene->ForEachAsync<TransformComponent, RigidBodyComponent>(
        [=](fr::Entity entity, TransformComponent& transform,
            RigidBodyComponent& rigidBody) {
            if (glm::length(rigidBody.velocity) > 0)
            {
                mScene->SendEvent(TransformChangeEvent { .entity = entity });
                transform.position += rigidBody.velocity * deltaTime;
            }

            rigidBody.velocity *= 1.0f - 0.8f * deltaTime;
        });
}