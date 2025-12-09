#pragma once

#include <Freyr/Freyr.hpp>

#include "Components/RigidBodyComponent.hpp"

#include <Containers/Octree.hpp>

class OctreeSystem final : public fr::System
{
  public:
    explicit OctreeSystem(const Ref<fr::Scene>& scene) : System(scene), mAllocator(new ArenaAllocator())
    {
        BuildOctree();
    }

    ~OctreeSystem() override = default;

    void PreFixedUpdate(float deltaTime) override;

    Octree* GetOctree() const { return mOctree; }

  private:
    void BuildOctree();

    ArenaAllocator* mAllocator;
    Octree* mOctree;
};
