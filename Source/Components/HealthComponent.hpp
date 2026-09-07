#pragma once

#include <Freyr/Freyr.hpp>

struct HealthComponent : fr::Component
{
    float hitPoints    = 1000.0f;
    float maxHitPoints = 1000.0f;
};
