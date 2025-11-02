#pragma once

#include <Freyr/Freyr.hpp>

struct  LaserGunComponent : fr::Component
{
    bool triggered;
    float fireRate;
    float fireTime;
    float energyCost;
    float maxEnergy;
};

struct BulletComponent : fr::Component
{
    fr::Entity owner;
};
