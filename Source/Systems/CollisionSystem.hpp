#pragma once

#include "Events/TransformChangeEvent.hpp"

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

#include <Systems/OctreeSystem.hpp>

class CollisionSystem final : public fr::System
{
  public:
    CollisionSystem(const Ref<fr::Scene>&     scene,
                    const Ref<fra::Renderer>& renderer,
                    const Ref<fra::MeshPool>& meshPool,
                    const Ref<OctreeSystem>&  octreeSystem) :
        System(scene), mRenderer(renderer), mMeshPool(meshPool),
        mOctreeSystem(octreeSystem), mChangedEntities(1000)
    {
        mScene->AddEventListener<TransformChangeEvent>(
            [this](const TransformChangeEvent& e) {
                mChangedEntities.insert(e.entity);
            });
    }

    ~CollisionSystem() override = default;

    void FixedUpdate(float deltaTime) override;

  private:
    friend class SpaceApp;

    fr::SparseSet<fr::Entity> mChangedEntities;

    std::vector<std::uint32_t> mSphereModel;
    std::vector<std::uint32_t> mCubeModel;

    Ref<fra::Renderer> mRenderer;
    Ref<fra::MeshPool> mMeshPool;
    Ref<OctreeSystem>  mOctreeSystem;
};