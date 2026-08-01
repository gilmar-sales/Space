#pragma once

#include <Freyr/Freyr.hpp>

class SpaceShipSystem final : public fr::System
{
  public:
    explicit SpaceShipSystem(const skr::Arc<fr::Registry>& registry) : System(registry) {}

    ~SpaceShipSystem() override = default;

    void Update(float deltaTime) override;
};