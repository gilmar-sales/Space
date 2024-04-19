#pragma once

#include <Freyr.hpp>

class PhysicsSystem : public fr::System
{
  public:
    void Start() override;
    void Update(float deltaTime) override;
};
