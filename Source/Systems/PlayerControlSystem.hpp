#pragma once

#include <Freyr/Freyr.hpp>

class PlayerControlSystem : public fr::System
{
  public:
    void Start() override;

    void PostUpdate(float deltaTime) override;

  private:
    fr::Entity mPlayer;
    float mYawTorque;
};