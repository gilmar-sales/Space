#pragma once

#include <Freyr/Freyr.hpp>

#include "Components/RigidBodyComponent.hpp"
#include "Events/TransformChangeEvent.hpp"

#include <Containers/Octree.hpp>

class OctreeSystem final : public fr::System
{
  public:
    explicit OctreeSystem(const Ref<fr::Scene>& scene) :
        System(scene), mOctree(nullptr), mChangedEntities(1000)
    {
        mScene->AddEventListener<TransformChangeEvent>(
            [this](const TransformChangeEvent& e) {
                mChangedEntities.insert(e.entity);
            });

        auto allocator = std::allocator<Octree>();

        mOctree =
            std::make_shared<Octree>(glm::vec3(0), 200'000.0f, 6, allocator);

        mScene->ForEach<TransformComponent,
                        SphereColliderComponent,
                        RigidBodyComponent>(
            "Build Octree",
            [octree = mOctree](const fr::Entity         entity,
                               TransformComponent&      transform,
                               SphereColliderComponent& sphereCollider,
                               RigidBodyComponent&      rigidBody) {
                rigidBody.octree = octree->Insert(Particle {
                    .entity         = entity,
                    .transform      = &transform,
                    .sphereCollider = &sphereCollider });
            });
    }

    ~OctreeSystem() override = default;

    void PreFixedUpdate(float deltaTime) override;

    Ref<Octree>                      GetOctree() const { return mOctree; }
    const fr::SparseSet<fr::Entity>& GetChangedEntities() const
    {
        return mChangedEntities;
    }

  private:
    Ref<Octree>               mOctree;
    fr::SparseSet<fr::Entity> mChangedEntities;
};
