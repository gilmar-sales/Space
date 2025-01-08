#pragma once

#include "Events/ApplyForceEvent.hpp"
#include "Events/ApplyTorqueEvent.hpp"

#include <Freyr/Freyr.hpp>

#include <Containers/Octree.hpp>

class OctreeSystem final : public fr::System
{
  public:
    explicit OctreeSystem(const std::shared_ptr<fr::Scene>& scene) :
        System(scene), mOctree(nullptr), mChangedEntities(1000)
    {
        mScene->AddEventListener<ApplyForceEvent>(
            [this](const ApplyForceEvent& e) {
                mChangedEntities.insert(e.target);
            });
        mScene->AddEventListener<ApplyTorqueEvent>(
            [this](const ApplyTorqueEvent& e) {
                mChangedEntities.insert(e.target);
            });
    }

    ~OctreeSystem() override = default;

    void PreUpdate(float deltaTime) override;

    std::shared_ptr<Octree>          GetOctree() const { return mOctree; }
    const fr::SparseSet<fr::Entity>& GetChangedEntities() const
    {
        return mChangedEntities;
    }

  private:
    std::shared_ptr<Octree>   mOctree;
    fr::SparseSet<fr::Entity> mChangedEntities;
};