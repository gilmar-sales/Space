#include "OctreeSystem.hpp"

#include "Components/RigidBodyComponent.hpp"

void OctreeSystem::PreUpdate(float deltaTime)
{
    if (mOctree == nullptr)
    {
        auto allocator = std::allocator<Octree>();

        mOctree =
            std::make_shared<Octree>(glm::vec3(0), 200'000.0f, 4, allocator);

        mScene->ForEachParallel<TransformComponent, SphereColliderComponent,
                                RigidBodyComponent>(
            "Build Octree",
            [octree = mOctree](const fr::Entity entity, int index,
                               TransformComponent&      transform,
                               SphereColliderComponent& sphereCollider,
                               RigidBodyComponent&      rigidBody) {
                rigidBody.octree = octree->Insert(Particle {
                    .entity         = entity,
                    .transform      = &transform,
                    .sphereCollider = &sphereCollider });
            });

        return;
    }

    mChangedEntities.sort();

    mScene->ForEachParallel<TransformComponent, SphereColliderComponent,
                            RigidBodyComponent>(
        "Rebuild changed entities",
        mChangedEntities,
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

                if (rigidBody.octree != nullptr &&
                    !rigidBody.octree->Contains(particle))
                {
                    rigidBody.octree->Remove(entity);

                    rigidBody.octree = octree->Insert(particle);
                }
            }
        });

    mChangedEntities.size();
}