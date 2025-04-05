#pragma once

#include <Freya/Freya.hpp>
#include <Freyr/Freyr.hpp>

class PlayerControlSystem final : public fr::System
{
  public:
    PlayerControlSystem(const Ref<fr::Scene>&         scene,
                        const Ref<fra::EventManager>& eventManger);

    void PostUpdate(float deltaTime) override;

  private:
    fr::Entity mPlayer;
};