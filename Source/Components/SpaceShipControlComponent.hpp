#pragma once

#include <Freyr/Freyr.hpp>

constexpr auto MaxThrust       = 5'000;
constexpr auto BoostFactor = 2.f;
constexpr auto TurnTorque  = 60.0f;

struct SpaceShipControlComponent : fr::Component
{
    float boostFactor    = 1.0f;
    float throttle          = 0.0f;
    float rollTorque     = 0.0f;
    float pitchTorque    = 0.0f;
    float yawTorque      = 0.0f;
    bool  volatileTorque = false;
};