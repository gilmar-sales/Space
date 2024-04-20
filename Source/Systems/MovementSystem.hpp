#pragma once

#include <Freyr/Freyr.hpp>

class MovementSystem : public fr::System
{
  public:
    void Update(float deltaTime) override;
};