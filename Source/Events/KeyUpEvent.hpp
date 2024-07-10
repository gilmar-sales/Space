#pragma once

#include <Freyr/Freyr.hpp>

struct KeyUpEvent : fr::Event
{
    SDL_Scancode scancode;
};