#pragma once

#include <Freyr/Freyr.hpp>

class PlayerControlSystem final : public fr::System
{
  public:
    PlayerControlSystem() :
        mPlayer(), mYawTorque()
    {
    }
    void Start() override;

    void PostUpdate(float deltaTime) override;

  private:
    fr::Entity mPlayer;
    float mYawTorque;
};