#pragma once

#include <Freyr/Freyr.hpp>

class EnemyControlSystem final : public fr::System
{
  public:
    explicit EnemyControlSystem(const Ref<fr::Scene>& scene);

    void Update(float deltaTime) override;

  private:
    fr::Entity mPlayer;
};