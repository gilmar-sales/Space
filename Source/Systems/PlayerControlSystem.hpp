#pragma once

#include <Freyr/Freyr.hpp>

class PlayerControlSystem final : public fr::System
{
  public:
    PlayerControlSystem(const std::shared_ptr<fr::Scene>& scene);

    void PostUpdate(float deltaTime) override;

  private:
    fr::Entity mPlayer;
    float mYawTorque;
};