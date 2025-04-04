#pragma once

#include "Events/TransformChangeEvent.hpp"

#include <Freyr/Freyr.hpp>

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
    }

    ~OctreeSystem() override = default;

    void PreUpdate(float deltaTime) override;

    Ref<Octree>                      GetOctree() const { return mOctree; }
    const fr::SparseSet<fr::Entity>& GetChangedEntities() const
    {
        return mChangedEntities;
    }

  private:
    Ref<Octree>               mOctree;
    fr::SparseSet<fr::Entity> mChangedEntities;
};
