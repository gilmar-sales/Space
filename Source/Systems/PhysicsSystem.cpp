#include "PhysicsSystem.hpp"

#include <Components/RigidBodyComponent.hpp>
#include <Components/SphereColliderComponent.hpp>
#include <Components/TransformComponent.hpp>

#include <Events/ApplyForceEvent.hpp>
#include <Events/ApplyTorqueEvent.hpp>
#include <Events/CollisionEvent.hpp>

PhysicsSystem::PhysicsSystem(const Ref<fr::Scene>& scene) : System(scene)
{
    mScene->AddEventListener<CollisionEvent>([scene = mScene](const CollisionEvent collisionEvent) {
        scene->TryGetComponents<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            collisionEvent.target,
            [&](TransformComponent& targetTransform, SphereColliderComponent& targetCollider,
                RigidBodyComponent& targetRigidBody) {
                scene->TryGetComponents<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
                    collisionEvent.collisor,
                    [&](TransformComponent& collisorTransform, SphereColliderComponent& collisorCollider,
                        RigidBodyComponent& collisorRigidBody) {
                        const auto distance = glm::distance(targetTransform.position, collisorTransform.position);

                        const auto totalForce = (targetCollider.radius + collisorCollider.radius - distance);

                        const auto targetForce =
                            totalForce * (collisorRigidBody.mass / (targetRigidBody.mass + collisorRigidBody.mass));

                        const auto collisorForce =
                            totalForce * (targetRigidBody.mass / (targetRigidBody.mass + collisorRigidBody.mass));

                        const auto direction = normalize(targetTransform.position - collisorTransform.position);

                        targetTransform.position += direction * targetForce;
                        collisorTransform.position += -direction * collisorForce;

                        scene->SendEvent(ApplyForceEvent {
                            .target     = collisionEvent.target,
                            .direction  = glm::normalize(targetTransform.position - collisorTransform.position),
                            .magnetiude = targetForce,
                            .deltaTime  = collisionEvent.deltaTime });

                        scene->SendEvent(ApplyForceEvent {
                            .target     = collisionEvent.collisor,
                            .direction  = glm::normalize(collisorTransform.position - targetTransform.position),
                            .magnetiude = collisorForce,
                            .deltaTime  = collisionEvent.deltaTime });
                    });
            });
    });

    mScene->AddEventListener<ApplyForceEvent>([this](const ApplyForceEvent& applyForceEvent) {
        mScene->TryGetComponents<RigidBodyComponent>(applyForceEvent.target, [&](RigidBodyComponent& rigidBody) {
            const auto acceleration = applyForceEvent.magnetiude / rigidBody.mass;

            rigidBody.velocity += applyForceEvent.direction * acceleration * applyForceEvent.deltaTime;
        });
    });

    mScene->AddEventListener<ApplyTorqueEvent>([this](const ApplyTorqueEvent& applyTorqueEvent) {
        mScene->TryGetComponents<TransformComponent, RigidBodyComponent>(
            applyTorqueEvent.target, [&](TransformComponent& transform, RigidBodyComponent& rigidBody) {
                const auto angularAcceleration =
                    glm::radians(applyTorqueEvent.magnetiude / rigidBody.mass * applyTorqueEvent.deltaTime);

                transform.rotation *= glm::angleAxis(angularAcceleration, applyTorqueEvent.axis);
            });
    });
}

void PhysicsSystem::FixedUpdate(float deltaTime)
{
    mScene->ForEachAsync<TransformComponent, RigidBodyComponent>(
        [scene = mScene, deltaTime](fr::Entity entity, TransformComponent& transform, RigidBodyComponent& rigidBody) {
            transform.position += rigidBody.velocity * deltaTime;

            if (rigidBody.mass > 0)
                rigidBody.velocity *= 1.0f - 0.8f * deltaTime;
        });
}