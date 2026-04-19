#pragma once

#include <Freyr/Freyr.hpp>

constexpr auto Boost       = 5'000;
constexpr auto BoostFactor = 2.f;
constexpr auto TurnTorque  = 90.0f;

struct SpaceShipControlComponent : fr::Component
{
    float boostFactor    = 1.0f;
    float boost          = 0.0f;
    float rollTorque     = 0.0f;
    float pitchTorque    = 0.0f;
    float yawTorque      = 0.0f;
    bool  volatileTorque = false;
};