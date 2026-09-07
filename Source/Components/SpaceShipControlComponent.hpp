#pragma once

#include <Freyr/Freyr.hpp>

constexpr auto MaxThrust              = 9'000;
constexpr auto BoostFactor            = 3.0f;
constexpr auto MaxAngularSpeed        = 90.0f;
constexpr auto AngularAcceleration    = 240.0f;
constexpr auto AngularDamping         = 1.0f;

struct SpaceShipControlComponent : fr::Component
{
    float boostFactor = 1.0f;
    float throttle    = 0.0f;
    float rollInput   = 0.0f;
    float pitchInput  = 0.0f;
    float yawInput    = 0.0f;

    // Mouse input is an impulse-like input and is cleared after each simulation step.
    bool volatileInput = false;
};