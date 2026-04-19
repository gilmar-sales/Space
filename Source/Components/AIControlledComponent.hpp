#pragma once

#include <Freyr/Freyr.hpp>

enum class Behaviour
{
    Patrol,
    Chase,
    Flee
};

struct AIControlledComponent : fr::Component
{
    Behaviour  behaviour;
    fr::Entity target;
};