#pragma once

#include <Freyr/Freyr.hpp>

class PhysicsSystem final : public fr::System
{
  public:
    virtual ~PhysicsSystem() = default;

    void Start() override;
    void Update(float deltaTime) override;
};
