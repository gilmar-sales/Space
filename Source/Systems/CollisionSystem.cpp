#include "CollisionSystem.hpp"

#include "Events/CollisionEvent.hpp"

#include "Components/RigidBodyComponent.hpp"
#include "Components/SphereColliderComponent.hpp"
#include "Components/TransformComponent.hpp"

void CollisionSystem::FixedUpdate(float deltaTime)
{
    auto octree = mOctreeSystem->GetOctree();

    mScene->ForEachAsync<TransformComponent, SphereColliderComponent,
                         RigidBodyComponent>(
        "Calculate collisions",
        [this, octree = octree, manager = mScene, deltaTime = deltaTime](
            const fr::Entity entity, TransformComponent& transform,
            SphereColliderComponent& sphereCollider,
            RigidBodyComponent&      rigidBody) {
            if (rigidBody.isKinematic)
                return;

            auto collisions = std::vector<Particle>(0);

            auto particle = Particle { .entity         = entity,
                                       .transform      = &transform,
                                       .sphereCollider = &sphereCollider };

            octree->Query(particle, collisions);

            for (const auto collision : collisions)
            {
                manager->SendEvent<CollisionEvent>(CollisionEvent {
                    .target    = entity,
                    .collisor  = collision.entity,
                    .deltaTime = deltaTime });
            }
        });
}
