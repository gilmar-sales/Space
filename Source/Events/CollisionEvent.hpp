#pragma once

#include <Freyr/Freyr.hpp>

struct CollisionEvent : fr::Event
{
    fr::Entity target;
    fr::Entity collisor;
    float      deltaTime;
};
