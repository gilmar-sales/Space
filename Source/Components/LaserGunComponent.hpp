#pragma once

struct LaserGunComponent  {
    bool triggered;
    float fireRate;
    float fireTime;
    float energyCost;
    float energySpent;
    float maxEnergy;
    std::uint32_t materialId;
};

struct BulletComponent  {
    fr::Entity owner;
};
