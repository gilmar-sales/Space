#pragma once

#include <Freyr/Freyr.hpp>

#include <Containers/Octree.hpp>

class OctreeSystem final : public fr::System
{
  public:
    OctreeSystem(const std::shared_ptr<fr::Scene>& scene) :
        System(scene), mOctree(nullptr)
    {
    }

    ~OctreeSystem() override = default;

    void PreUpdate(float deltaTime) override;

    std::shared_ptr<Octree> GetOctree() const { return mOctree; }

  private:
    std::shared_ptr<Octree> mOctree;
};