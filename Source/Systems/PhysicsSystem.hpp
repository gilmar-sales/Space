#pragma once

#include <Freyr/Freyr.hpp>

#include "OctreeSystem.hpp"

class PhysicsSystem final : public fr::System
{
  public:
    explicit PhysicsSystem(const skr::Arc<fr::Registry>& registry, const skr::Arc<OctreeSystem>& octreeSystem);

    ~PhysicsSystem() override = default;

    void Update(float deltaTime) override;

  private:
    skr::Arc<OctreeSystem>       mOctreeSystem;
    skr::Arc<fr::ListenerHandle> mCollisionListener;
};
