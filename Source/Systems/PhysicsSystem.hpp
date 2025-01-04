#pragma once

#include <Freyr/Freyr.hpp>

class PhysicsSystem final : public fr::System
{
  public:
    explicit PhysicsSystem(const std::shared_ptr<fr::Scene>& scene);

    ~PhysicsSystem() override = default;

    void Update(float deltaTime) override;
};
