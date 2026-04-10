#pragma once

#include <Freyr/Freyr.hpp>

#include "OctreeSystem.hpp"

class PhysicsSystem final : public fr::System
{
  public:
    explicit PhysicsSystem(const Ref<fr::Scene>& scene, const Ref<OctreeSystem>& octreeSystem);

    ~PhysicsSystem() override = default;

    void FixedUpdate(float deltaTime) override;

  private:
    Ref<OctreeSystem>       mOctreeSystem;
    Ref<fr::ListenerHandle> mCollisionListener;
};
