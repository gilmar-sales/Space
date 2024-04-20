#pragma once

#include <Freyr/Freyr.hpp>

#include <Containers/Octree.hpp>

class OctreeSystem : public fr::System
{
  public:
    void PreUpdate(float deltaTime) override;

    std::shared_ptr<Octree> GetOctree()
    {
        return mOctree;
    }

  private:
    std::shared_ptr<Octree> mOctree;
};