#pragma once

#include <Freyr/Freyr.hpp>

class SpaceShipSystem final : public fr::System
{
  public:
    explicit SpaceShipSystem(const Ref<fr::Scene>& scene) : System(scene) {}

    ~SpaceShipSystem() override = default;

    void Update(float deltaTime) override;
};