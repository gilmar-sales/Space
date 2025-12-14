#include "PhysicsSystem.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Events/CollisionEvent.hpp"

PhysicsSystem::PhysicsSystem(const Ref<fr::Scene>& scene, const Ref<OctreeSystem>& octreeSystem) :
    System(scene), mOctreeSystem(octreeSystem)
{
    mCollisionListener =
        mScene->AddEventListener<CollisionEvent>([scene = mScene](const CollisionEvent collisionEvent) {
            scene->TryGetComponents<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
                collisionEvent.target,
                [&](TransformComponent& targetTransform, const SphereColliderComponent& targetCollider,
                    RigidBodyComponent& targetRigidBody) {
                    scene->TryGetComponents<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
                        collisionEvent.collisor,
                        [&](TransformComponent& collisorTransform, const SphereColliderComponent& collisorCollider,
                            RigidBodyComponent& collisorRigidBody) {
                            const auto distance = glm::distance(targetTransform.position, collisorTransform.position);

                            const auto totalForce = (targetCollider.radius + collisorCollider.radius - distance);

                            const auto totalMass = targetRigidBody.mass + collisorRigidBody.mass;

                            const auto targetForce = totalForce * (collisorRigidBody.mass / totalMass);

                            const auto collisorForce = totalForce * (targetRigidBody.mass / totalMass);

                            const auto direction = normalize(targetTransform.position - collisorTransform.position);

                            targetRigidBody.ApplyForce(direction, targetForce, collisionEvent.deltaTime);
                            collisorRigidBody.ApplyForce(-direction, collisorForce, collisionEvent.deltaTime);
                        });
                });
        });
}

void PhysicsSystem::FixedUpdate(float deltaTime)
{
    mScene->ForEachAsync<TransformComponent, RigidBodyComponent, SphereColliderComponent>(
        [this, deltaTime](fr::Entity entity, TransformComponent& transform, RigidBodyComponent& rigidBody,
                          SphereColliderComponent& sphereCollider) {
            if (rigidBody.isKinematic)
                return;

            if (rigidBody.kinematicIfStop && glm::length(rigidBody.velocity) <= 0.01f)
            {
                rigidBody.isKinematic     = true;
                rigidBody.kinematicIfStop = false;
                mOctreeSystem->InsertKinematic(
                    Particle { .entity = entity, .transform = transform, .sphereCollider = sphereCollider });
                return;
            }

            transform.position += rigidBody.velocity * deltaTime;

            if (rigidBody.mass > 0)
            {
                rigidBody.velocity *= glm::pow(1.0f - 0.8f, deltaTime);
            }
        });
}
