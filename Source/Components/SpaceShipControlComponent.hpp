#pragma once

#include <Freyr/Freyr.hpp>

constexpr auto MaxThrust           = 9'000;
constexpr auto BoostFactor         = 3.0f;
constexpr auto MaxAngularSpeed     = 90.0f;  // degrees / second
constexpr auto AngularAcceleration = 240.0f; // degrees / second^2
constexpr auto AngularDamping      = 8.0f;   // exponential decay rate when idle
constexpr auto MouseDegreesPerPixel = 0.12f;

struct SpaceShipControlComponent : fr::Component
{
    float boostFactor = 1.0f;
    float throttle    = 0.0f;

    // Continuous stick / key / AI intent in [-1, 1].
    float rollInput  = 0.0f;
    float pitchInput = 0.0f;
    float yawInput   = 0.0f;

    // Mouse look accumulates desired degrees and is consumed once per sim step.
    float pitchImpulse = 0.0f;
    float yawImpulse   = 0.0f;
};
