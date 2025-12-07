#pragma once

#include <Freyr/Freyr.hpp>

constexpr auto Boost       = 6'000;
constexpr auto BoostFactor = 3.3f;
constexpr auto TurnTorque  = 90.0f;

struct SpaceShipControlComponent : fr::Component
{
    float boostFactor = 1.0f;
    float boost;
    float rollTorque;
    float pitchTorque;
    float yawTorque;
};