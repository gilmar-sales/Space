#pragma once

#include <Freyr.hpp>

class MovementSystem : public fr::System
{
  public:
    void Update(float deltaTime) override;
};