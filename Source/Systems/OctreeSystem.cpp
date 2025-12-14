#include "OctreeSystem.hpp"

void OctreeSystem::PreFixedUpdate(float deltaTime)
{
    BuildOctree();
}

void OctreeSystem::BuildOctree()
{
    if (mKinematicOctree == nullptr)
    {
        mKinematicAllocator->reset();
        mKinematicOctree = mKinematicAllocator->construct<Octree>(glm::vec3(0), 200'000.0f, mKinematicAllocator);

        mScene->ForEachAsync<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
            "Build Kinematic Octree",
            [this](const fr::Entity         entity,
                   TransformComponent&      transform,
                   SphereColliderComponent& sphereCollider,
                   RigidBodyComponent&      rigidBody) {
                if (!rigidBody.isKinematic)
                    return;

                Entry entry = {
                    .entity = entity,
                    .node   = mKinematicOctree->Insert(
                        Particle { .entity = entity, .transform = transform, .sphereCollider = sphereCollider })
                };
                mEntries.insert(entry);
            });
    }

    mAllocator->reset();
    mOctree = mAllocator->construct<Octree>(glm::vec3(0), 200'000.0f, mAllocator);

    mScene->ForEachAsync<TransformComponent, SphereColliderComponent, RigidBodyComponent>(
        "Build Octree",
        [this](const fr::Entity         entity,
               TransformComponent&      transform,
               SphereColliderComponent& sphereCollider,
               RigidBodyComponent&      rigidBody) {
            if (rigidBody.isKinematic)
                return;

            mOctree->Insert(Particle { .entity = entity, .transform = transform, .sphereCollider = sphereCollider });
        });
}

void OctreeSystem::Query(Particle& particle, std::vector<Particle>& found)
{
    if (mKinematicOctree != nullptr)
        mKinematicOctree->Query(particle, found);

    if (mOctree != nullptr)
        mOctree->Query(particle, found);
}