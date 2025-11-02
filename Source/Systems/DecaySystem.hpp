#pragma once

#include <Freyr/Freyr.hpp>

class DecaySystem : public fr::System
{
    public:
    DecaySystem(const Ref<fr::Scene>&     scene)  : fr::System(scene) {}

    void PreUpdate(float deltaTime) override;
};