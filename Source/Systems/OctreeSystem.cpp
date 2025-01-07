#include "OctreeSystem.hpp"

#include "Components/RigidBodyComponent.hpp"

void OctreeSystem::PreUpdate(float deltaTime)
{
    if (mOctree == nullptr)
    {
        auto allocator = std::allocator<Octree>();

        mOctree =
            std::make_shared<Octree>(glm::vec3(0), 100'000.0f, 6, allocator);

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

    mScene->ForEachParallel<TransformComponent, SphereColliderComponent,
                            RigidBodyComponent>(
        "Build Octree",
        [octree = mOctree](
            const fr::Entity entity, int index, TransformComponent& transform,
            SphereColliderComponent& sphereCollider,
            RigidBodyComponent&      rigidBody) {
            if (!rigidBody.isKinematic)
            {
                if (rigidBody.octree != nullptr)
                    rigidBody.octree->Remove(entity);

                rigidBody.octree = octree->Insert(Particle {
                    .entity         = entity,
                    .transform      = &transform,
                    .sphereCollider = &sphereCollider });
            }
        });
}