#pragma once

#include <Freyr/Freyr.hpp>

struct LaserGunComponent : fr::Component {
    bool triggered;
    float fireRate;
    float fireTime;
    float energyCost;
    float energySpent;
    float maxEnergy;
    std::uint32_t materialId;
};

struct BulletComponent : fr::Component {
    fr::Entity owner;
};
