#include "PhysicsSystem.hpp"

#include <Components/RigidBodyComponent.hpp>
#include <Components/SphereColliderComponent.hpp>
#include <Components/TransformComponent.hpp>

#include <Events/CollisionEvent.hpp>

PhysicsSystem::PhysicsSystem(const Ref<fr::Scene>& scene) : System(scene)
{
    mCollisionListener =
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

                            targetRigidBody.ApplyForce(direction, targetForce, collisionEvent.deltaTime);
                            collisorRigidBody.ApplyForce(-direction, collisorForce, collisionEvent.deltaTime);
                        });
                });
        });
}

void PhysicsSystem::FixedUpdate(float deltaTime)
{
    mScene->ForEachAsync<TransformComponent, RigidBodyComponent>(
        [scene = mScene, deltaTime](fr::Entity entity, TransformComponent& transform, RigidBodyComponent& rigidBody) {
            transform.position += rigidBody.velocity * deltaTime;

            if (rigidBody.mass > 0)
            {
                rigidBody.velocity *= glm::pow(1.0f - 0.8f, deltaTime);
            }
        });
}
