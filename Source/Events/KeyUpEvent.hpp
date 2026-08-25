#pragma once

#include <Freyr/Freyr.hpp>

#include <SDL3/SDL.h>

struct KeyUpEvent : fr::Event
{
    SDL_Scancode scancode;
};