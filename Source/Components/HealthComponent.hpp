#pragma once

#include <Freyr/Freyr.hpp>

struct HealthComponent : fr::Component
{
    float hitPoints;
    float regenerationRate;
};