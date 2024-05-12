#pragma once

#include <Freyr/Freyr.hpp>

#include <Containers/Octree.hpp>

class OctreeSystem : public fr::System
{
  public:
    virtual ~OctreeSystem() = default;

    void PreUpdate(float deltaTime) override;

    std::shared_ptr<Octree> GetOctree() const { return mOctree; }

  private:
    std::shared_ptr<Octree> mOctree;
};