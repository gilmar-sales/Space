#pragma once

#include <Freyr/Freyr.hpp>

struct KeyDownEvent : fr::Event
{
    SDL_Scancode scancode;
};