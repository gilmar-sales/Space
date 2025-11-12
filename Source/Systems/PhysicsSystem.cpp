#include "PhysicsSystem.hpp"

#include "Events/TransformChangeEvent.hpp"

#include <Components/RigidBodyComponent.hpp>
#include <Components/SphereColliderComponent.hpp>
#include <Components/TransformComponent.hpp>

#include <Events/ApplyForceEvent.hpp>
#include <Events/ApplyTorqueEvent.hpp>
#include <Events/CollisionEvent.hpp>

PhysicsSystem::PhysicsSystem(const Ref<fr::Scene>& scene) : System(scene)
{
    mScene->AddEventListener<CollisionEvent>([scene = mScene](const CollisionEvent collisionEvent) {
        scene->TryGetComponents<TransformComponent, SphereColliderComponent>(
            collisionEvent.target,
            [&](TransformComponent& targetTransform, SphereColliderComponent& targetCollider) {
                scene->TryGetComponents<TransformComponent, SphereColliderComponent>(
                    collisionEvent.collisor,
                    [&](TransformComponent& collisorTransform, SphereColliderComponent& collisorCollider) {
                        const auto distance = glm::distance(targetTransform.position, collisorTransform.position);

                        const auto force = (targetCollider.radius + collisorCollider.radius - distance) / 2;

                        const auto direction = normalize(targetTransform.position - collisorTransform.position);

                        targetTransform.position += direction * force;
                        collisorTransform.position += -direction * force;

                        scene->SendEvent(ApplyForceEvent {
                            .target     = collisionEvent.target,
                            .direction  = glm::normalize(targetTransform.position - collisorTransform.position),
                            .magnetiude = force,
                            .deltaTime  = collisionEvent.deltaTime });

                        scene->SendEvent(ApplyForceEvent {
                            .target     = collisionEvent.collisor,
                            .direction  = glm::normalize(collisorTransform.position - targetTransform.position),
                            .magnetiude = force,
                            .deltaTime  = collisionEvent.deltaTime });
                    });
            });
    });

    mScene->AddEventListener<ApplyForceEvent>([this](const ApplyForceEvent& applyForceEvent) {
        mScene->TryGetComponents<RigidBodyComponent>(applyForceEvent.target, [&](RigidBodyComponent& rigidBody) {
            const auto acceleration = applyForceEvent.magnetiude / rigidBody.mass;

            rigidBody.velocity += applyForceEvent.direction * acceleration * applyForceEvent.deltaTime;

            if (acceleration > 0.01f)
                mScene->SendEvent(TransformChangeEvent { .entity = applyForceEvent.target });
        });
    });

    mScene->AddEventListener<ApplyTorqueEvent>([this](const ApplyTorqueEvent& applyTorqueEvent) {
        mScene->TryGetComponents<TransformComponent, RigidBodyComponent>(
            applyTorqueEvent.target, [&](TransformComponent& transform, RigidBodyComponent& rigidBody) {
                const auto angularAcceleration =
                    glm::radians(applyTorqueEvent.magnetiude / rigidBody.mass * applyTorqueEvent.deltaTime);

                transform.rotation *= glm::angleAxis(angularAcceleration, applyTorqueEvent.axis);

                if (angularAcceleration > 0.01f)
                    mScene->SendEvent(TransformChangeEvent { .entity = applyTorqueEvent.target });
            });
    });

    mScene->ForEach<TransformComponent, RigidBodyComponent>(
        [scene = mScene](fr::Entity entity, TransformComponent& transform, RigidBodyComponent& rigidBody) {
            scene->SendEvent(TransformChangeEvent { .entity = entity });
        });
}
void PhysicsSystem::FixedUpdate(float deltaTime)
{
    mScene->ForEachAsync<TransformComponent, RigidBodyComponent>(
        [scene = mScene, deltaTime](fr::Entity entity, TransformComponent& transform, RigidBodyComponent& rigidBody) {
            if (glm::length(rigidBody.velocity) > 0.01f)
            {
                scene->SendEvent(TransformChangeEvent { .entity = entity });
                transform.position += rigidBody.velocity * deltaTime;
            }

            if (rigidBody.mass > 0)
                rigidBody.velocity *= 1.0f - 0.8f * deltaTime;
        });
}