#pragma once

#include <Freyr/Freyr.hpp>

enum class Behaviour : uint8_t
{
    Patrol,
    Chase,
    Flee
};

enum class CombatManeuver : uint8_t
{
    Pursue,
    Orbit,
    Strafe,
    Reposition
};

struct AIControlledComponent : fr::Component
{
    Behaviour      behaviour        = Behaviour::Patrol;
    CombatManeuver maneuver         = CombatManeuver::Pursue;
    fr::Entity     target           = 0;
    fr::Entity     squadLeader      = 0;
    float          fleeTime         = 0.0f;
    float          boostTime        = 0.0f;
    float          boostCooldown    = 0.0f;
    float          frustration      = 0.0f;
    float          retargetCooldown = 0.0f;
    float          maneuverTime     = 0.0f;

    float aggression         = 0.7f;
    float shootRangeScale    = 1.0f;
    float boostDurationScale = 1.0f;
    float orbitBias          = 0.5f;
    float flankSign          = 1.0f;
    bool  preferLead         = false;
};
