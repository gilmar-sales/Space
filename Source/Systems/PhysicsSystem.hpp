#pragma once

#include <Freyr/Freyr.hpp>

class PhysicsSystem : public fr::System
{
  public:
    void Start() override;
    void Update(float deltaTime) override;
};
