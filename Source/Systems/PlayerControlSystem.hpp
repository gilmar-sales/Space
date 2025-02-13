#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class PlayerControlSystem final : public fr::System
{
  public:
    PlayerControlSystem(const std::shared_ptr<fr::Scene>& scene, const std::shared_ptr<fra::EventManager>& eventManger);

    void PostUpdate(float deltaTime) override;

  private:
    fr::Entity mPlayer;
    float mYawTorque;
};