#include "OctreeSystem.hpp"

void OctreeSystem::PreFixedUpdate(float deltaTime)
{
    mScene->ForEachAsync<TransformComponent, SphereColliderComponent,
                         RigidBodyComponent>(
        "Rebuild changed entities",
        [octree = mOctree](
            const fr::Entity entity, TransformComponent& transform,
            SphereColliderComponent& sphereCollider,
            RigidBodyComponent&      rigidBody) {
            if (!rigidBody.isKinematic)
            {
                const auto particle =
                    Particle { .entity         = entity,
                               .transform      = &transform,
                               .sphereCollider = &sphereCollider };

                if (const auto actualOctree = rigidBody.octree;
                    actualOctree != nullptr &&
                    !rigidBody.octree->Contains(particle))
                {
                    rigidBody.octree = nullptr;
                    actualOctree->Remove(entity);

                    rigidBody.octree = octree->Insert(particle);
                }
            }
        });
}