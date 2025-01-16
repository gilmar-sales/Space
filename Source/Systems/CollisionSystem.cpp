#include "CollisionSystem.hpp"

#include <Asset/MeshPool.hpp>

#include "Events/CollisionEvent.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include <Events/OctreeFinishedEvent.hpp>

void CollisionSystem::Update(float deltaTime)
{
    auto octree = mOctreeSystem->GetOctree();

    mChangedEntities.sort();

    mScene->ForEachParallel<TransformComponent, SphereColliderComponent,
                            RigidBodyComponent>(
        "Calculate collisions", mChangedEntities,
        [octree = octree, manager = mScene, deltaTime = deltaTime](
            const fr::Entity entity, TransformComponent& transform,
            SphereColliderComponent& sphereCollider,
            RigidBodyComponent&      rigidBody) {
            auto collisions = std::vector<Particle*>(0);

            auto particle = Particle { .entity         = entity,
                                       .transform      = &transform,
                                       .sphereCollider = &sphereCollider };

            octree->Query(particle, collisions);

            for (const auto collision : collisions)
            {
                manager->SendEvent<CollisionEvent>(CollisionEvent {
                    .target    = entity,
                    .collisor  = collision->entity,
                    .deltaTime = deltaTime });
            }
        });

    mChangedEntities.clear();
}
