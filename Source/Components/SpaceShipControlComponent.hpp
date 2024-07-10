#pragma once

#include <Freyr/Freyr.hpp>

struct SpaceShipControlComponent : fr::Component
{
    float boost;
    float rollTorque;
    float pitchTorque;
    float yawTorque;
};