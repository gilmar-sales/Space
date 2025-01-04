#pragma once

#include <Freyr/Freyr.hpp>

class MovementSystem final : public fr::System
{
  public:
    explicit MovementSystem(const std::shared_ptr<fr::Scene>& scene) :
        System(scene)
    {
    }

    ~MovementSystem() override = default;

    void Update(float deltaTime) override;
};