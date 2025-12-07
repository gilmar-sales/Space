#pragma once

#include <Freyr/Freyr.hpp>

class PhysicsSystem final : public fr::System
{
  public:
    explicit PhysicsSystem(const Ref<fr::Scene>& scene);

    ~PhysicsSystem() override = default;

    void FixedUpdate(float deltaTime) override;

  private:
    Ref<fr::ListenerHandle> mCollisionListener;
};
