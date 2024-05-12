#pragma once

#include <Freyr/Freyr.hpp>

class MovementSystem : public fr::System
{
  public:
    virtual ~MovementSystem() = default;

    void Update(float deltaTime) override;
};