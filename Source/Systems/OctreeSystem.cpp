#include "OctreeSystem.hpp"
#include <Events/OctreeFinishedEvent.hpp>

void OctreeSystem::PreUpdate(float deltaTime)
{
    // if (mOctree == nullptr)
    {
        auto allocator = std::allocator<Octree>();
        mOctree =
            std::make_shared<Octree>(glm::vec3(0), 100'000.0f, 4, allocator);

        mManager->ForEachParallel<TransformComponent, SphereColliderComponent>(
            [octree = mOctree](const fr::Entity entity, int index,
                               TransformComponent&      transform,
                               SphereColliderComponent& sphereCollider) {
                octree->Insert(Particle { .entity         = entity,
                                          .transform      = transform,
                                          .sphereCollider = sphereCollider });
            });
    }
}