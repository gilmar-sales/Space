#include "PhysicsSystem.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Events/CollisionEvent.hpp"

PhysicsSystem::PhysicsSystem(const skr::Arc<fr::Registry>& registry, const skr::Arc<OctreeSystem>& octreeSystem) :
    System(registry), mOctreeSystem(octreeSystem)
{
    mCollisionListener =
        mRegistry->AddEventListener<CollisionEvent>([this](const CollisionEvent collisionEvent) {
            mRegistry->TryGetComponents<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
                collisionEvent.target,
                [&](TransformComponent& targetTransform, const SphereColliderComponent& targetCollider,
                    RigidBodyComponent& targetRigidBody) {
                    mRegistry->TryGetComponents<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
                        collisionEvent.collisor,
                        [&](TransformComponent& collisorTransform, const SphereColliderComponent& collisorCollider,
                            RigidBodyComponent& collisorRigidBody) {
                            const auto delta    = targetTransform.position - collisorTransform.position;
                            const auto distance = glm::length(delta);

                            const auto totalForce = (targetCollider.radius + collisorCollider.radius - distance);

                            const auto totalMass = targetRigidBody.mass + collisorRigidBody.mass;

                            const auto targetForce = totalForce * (collisorRigidBody.mass / totalMass);

                            const auto collisorForce = totalForce * (targetRigidBody.mass / totalMass);

                            const auto direction = delta / distance;

                            targetTransform.position += direction * targetForce;
                            targetRigidBody.ApplyForce(direction, targetForce, collisionEvent.deltaTime);

                            collisorTransform.position -= direction * collisorForce;
                            collisorRigidBody.ApplyForce(-direction, collisorForce, collisionEvent.deltaTime);
                        });
                });
        });
}

void PhysicsSystem::Update(float deltaTime)
{
    mRegistry->CreateMutation()->EachAsync<TransformComponent, RigidBodyComponent, SphereColliderComponent>(
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

            if (rigidBody.mass > 0.001f)
            {
                rigidBody.velocity *= 1.0f - 0.8f * deltaTime;
            }
        });
}
