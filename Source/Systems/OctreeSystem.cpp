#include "OctreeSystem.hpp"

void OctreeSystem::PreFixedUpdate(float deltaTime)
{
    BuildOctree();
}

void OctreeSystem::BuildOctree()
{
    mAllocator->reset();
    mOctree = mAllocator->construct<Octree>(glm::vec3(0), 200'000.0f, 6);

    mScene->ForEachAsync<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
        "Build Octree",
        [octree = mOctree](const fr::Entity         entity,
                           TransformComponent&      transform,
                           SphereColliderComponent& sphereCollider,
                           RigidBodyComponent&) {
            octree->Insert(Particle { .entity = entity, .transform = &transform, .sphereCollider = &sphereCollider });
        });
}