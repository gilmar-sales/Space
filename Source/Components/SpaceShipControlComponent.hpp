#pragma once

#include <Freyr/Freyr.hpp>

struct SpaceShipControlComponent : fr::Component
{
    float boostFactor = 1.0f;
    float boost;
    float rollTorque;
    float pitchTorque;
    float yawTorque;
};