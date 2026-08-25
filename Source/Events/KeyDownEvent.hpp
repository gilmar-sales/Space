#pragma once

#include <Freyr/Freyr.hpp>

#include <SDL3/SDL.h>

struct KeyDownEvent : fr::Event
{
    SDL_Scancode scancode;
};