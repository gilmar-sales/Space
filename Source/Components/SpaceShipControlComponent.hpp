#pragma once

#include <Freyr/Freyr.hpp>

struct SpaceShipControlComponent : fr::Component
{
    float rollTorque;
    float pitchTorque;
    float yawTorque;
};