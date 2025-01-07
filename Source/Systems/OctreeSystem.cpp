#include "OctreeSystem.hpp"

void OctreeSystem::PreUpdate(float deltaTime)
{
    auto allocator = std::allocator<Octree>();

    mOctree = std::make_shared<Octree>(glm::vec3(0), 100'000.0f, 6, allocator);

    mScene->ForEachParallel<TransformComponent, SphereColliderComponent>("Build Octree",
        [octree = mOctree](const fr::Entity entity, int index,
                           TransformComponent&      transform,
                           SphereColliderComponent& sphereCollider) {
            octree->Insert(Particle { .entity         = entity,
                                      .transform      = transform,
                                      .sphereCollider = sphereCollider });
        });
}