#pragma once

#include <Freyr/Freyr.hpp>

struct MouseMoveEvent : fr::Event
{
    float deltaX;
    float deltaY;
};