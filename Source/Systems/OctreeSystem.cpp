#include "OctreeSystem.hpp"
#include <Events/OctreeFinishedEvent.hpp>

void OctreeSystem::PreUpdate(float deltaTime)
{
    if (mOctree == nullptr)
    {
        mOctree = std::make_shared<Octree>(glm::vec3(0), 100'000.0f, 4);

        mManager->ForEachParallel<TransformComponent, SphereColliderComponent>(
            [octree = mOctree](fr::Entity entity, int index,
                               TransformComponent&      transform,
                               SphereColliderComponent& sphereCollider) {
                octree->Insert(Particle { .entity         = entity,
                                          .transform      = transform,
                                          .sphereCollider = sphereCollider });
            });
    }
}