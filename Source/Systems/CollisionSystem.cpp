#include "CollisionSystem.hpp"

#include <Asset/MeshPool.hpp>

#include "Events/CollisionEvent.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include <Events/OctreeFinishedEvent.hpp>

void CollisionSystem::Start()
{
}

void CollisionSystem::Update(float deltaTime)
{
    auto octree = mOctreeSystem->GetOctree();
    mManager->ForEachParallel<TransformComponent, SphereColliderComponent,
                           RigidBodyComponent>(
        "Calculate collisions",
        [octree = octree, manager = mManager, deltaTime = deltaTime](
            const fr::Entity entity, auto index, TransformComponent& transform,
            SphereColliderComponent& sphereCollider,
            RigidBodyComponent&      rigidBody) {
            auto collisions = std::vector<Particle*>(0);

            auto particle = Particle { .entity         = entity,
                                       .transform      = transform,
                                       .sphereCollider = sphereCollider };

            octree->Query(particle, collisions);

            for (const auto collision : collisions)
            {
                manager->SendEvent<CollisionEvent>(CollisionEvent {
                    .target    = entity,
                    .collisor  = collision->entity,
                    .deltaTime = deltaTime });
            }
        });
}
